#include <Arduino.h>
#include <PuzzleModule.h>
#include <Rules.h>
#include <map>

const int DIGITS = 5;

const unsigned long STAGE_LOAD_TIME = 1000;
unsigned long nextStageLoad = 0;

uint8_t segmentPins[7] = {32, 27, 12, 26, 25, 33, 14};
uint8_t digitPins[DIGITS] = {13, 15, 2, 4, 16};
uint8_t buttonPins[BUTTONS] = {36, 39, 34, 35};
uint8_t stageLEDPins[STAGES] = {17, 5, 18, 19, 21};
uint8_t redPin = 22, greenPin = 23;

Rule** rules = NULL;

struct Digit {
  uint8_t index;
  unsigned long time;
  bool on;
};

bool invertDigit[DIGITS] = {false, true, true, true, true};

int8_t segments[5] = {
  0b00000000,
  0b00000110,
  0b01011011,
  0b01001111,
  0b01100110,
};

EventQueue<Digit> eventQueue;

struct Stage {
  int stageNumber;
  int displayNumber;
  int buttonNumbers[BUTTONS];
} currentStage;

struct StageSolution {
  int number, index;
} stageSolutions[STAGES];

uint32_t random_range(uint32_t a, uint32_t b) {
  return esp_random() % (b - a + 1) + a;
}

void generateRandomStage(int stageNumber) {
  Stage newStage;
  newStage.stageNumber = stageNumber;
  newStage.displayNumber = random_range(1, 4);
  for (int i = 0; i < BUTTONS; i++) {
    newStage.buttonNumbers[i] = i + 1;
  }
  for (int i = 0; i < BUTTONS - 1; i++) {
    int j = random_range(i, BUTTONS - 1);
    int temp = newStage.buttonNumbers[i];
    newStage.buttonNumbers[i] = newStage.buttonNumbers[j];
    newStage.buttonNumbers[j] = temp;
  }
  currentStage = newStage;
}

void setup() {
  for (int i = 0; i < 7; i++) {
    pinMode(segmentPins[i], OUTPUT);
    digitalWrite(segmentPins[i], LOW);
  }

  for (int i = 0; i < DIGITS; i++) {
    pinMode(digitPins[i], OUTPUT);
    digitalWrite(digitPins[i], LOW);
  }

  for (int i = 0; i < BUTTONS; i++) {
    pinMode(buttonPins[i], INPUT);
  }

  for (int i = 0; i < STAGES; i++) {
    pinMode(stageLEDPins[i], OUTPUT);
    digitalWrite(stageLEDPins[i], LOW);
  }
  
  PuzzleModule::StatusLight statusLight;
  statusLight.init(redPin, greenPin);

  if (!PuzzleModule::init(statusLight)) {
    ESP.restart();
  }

  generateRandomStage(0);
}

void setSegments(int number, bool invert) {
  for (int i = 0; i < 7; i++) {
    bool result = ((~segments[number]) & (1 << i));
    digitalWrite(segmentPins[i], invert ? !result : result);
  }
}

void setDigit(int digit, bool state, bool invert) {
  digitalWrite(digitPins[digit], state ^ invert);
}

void updateDisplays() {
  if (eventQueue.isEmpty())
    return;
  Digit digit = eventQueue.front();
  if (digit.time > millis())
    return;
  eventQueue.pop();
  if (digit.on) {
    int number = (digit.index == 0) ? currentStage.displayNumber : currentStage.buttonNumbers[digit.index - 1];
    setSegments(number, invertDigit[digit.index]);
  }
  setDigit(digit.index, digit.on, invertDigit[digit.index]);
  Digit d;
  uint8_t nextDigit = (digit.index + 1) % 5;
  if (digit.on)
    d = {digit.index, millis() + 1, false};
  else
    d = {nextDigit, millis(), true};
  eventQueue.push(d);
}

void clearDisplays() {
  for (int i = 0; i < DIGITS; i++)
    setDigit(i, false, invertDigit[i]);
}

void result(bool solved, int number, int index) {
  eventQueue.clear();
  clearDisplays();
  nextStageLoad = millis() + STAGE_LOAD_TIME;
  if (solved) {
    stageSolutions[currentStage.stageNumber] = {number, index};
    digitalWrite(stageLEDPins[currentStage.stageNumber], HIGH);
    if (currentStage.stageNumber == STAGES - 1) {
      PuzzleModule::solve();
      currentStage.displayNumber = 0;
      for (int i = 0; i < BUTTONS; i++) {
        currentStage.buttonNumbers[i] = 0;
      }
      return;
    }
    generateRandomStage(currentStage.stageNumber + 1);
  } else {
    PuzzleModule::strike();
    generateRandomStage(0);
    for (int i = 0; i < STAGES; i++) {
      digitalWrite(stageLEDPins[i], LOW);
    }
  }
}

void check(Rule r, int number, int index) {
  if (r.rule == POSITION) {
    result(r.value == index, number, index);
  } else if (r.rule == LABEL) {
    result(r.value + 1 == number, number, index);
  } else if (r.rule == SAME_POSITION) {
    result(index == stageSolutions[r.value].index, number, index);
  } else if (r.rule == SAME_LABEL) {
    result(number == stageSolutions[r.value].number, number, index);
  }
}

void trySolve(int index) {
  int number = currentStage.buttonNumbers[index];
  Rule r = rules[currentStage.stageNumber][currentStage.displayNumber - 1];
  check(r, number, index);
}

unsigned long lastDebounceTime[BUTTONS];
bool lastButtonState[BUTTONS], buttonState[BUTTONS];
unsigned long debounceDelay = 50;

bool readButton(int index) {
  bool reading = digitalRead(buttonPins[index]);
  if (reading != lastButtonState[index]) {
    lastDebounceTime[index] = millis();
  }
  lastButtonState[index] = reading;
  if ((millis() - lastDebounceTime[index]) > debounceDelay) {
    if (reading != buttonState[index]) {
      buttonState[index] = reading;
    }
  }
  return buttonState[index];
}

int checkPressedButton() {
  if (millis() < nextStageLoad)
    return -1;
  for (int i = 0; i < BUTTONS; i++) {
    if (buttonState[i])
      return i;
  }
  return -1;
}

unsigned long lastAttempt = 0;
unsigned long attemptGap = 100;

void loop() {
  PuzzleModule::update();
  if (rules == NULL && millis() - lastAttempt > attemptGap) {
    lastAttempt = millis();
    PuzzleModule::withBombInfo([](BombInfo info) {
      rules = generate_rules(info.seed);
    });
    return;
  }
  if (millis() > nextStageLoad && eventQueue.isEmpty())
    eventQueue.push({0, millis()});
  updateDisplays();
  for (int i = 0; i < BUTTONS; i++)
    readButton(i);
  int pressed = checkPressedButton();
  if (pressed != -1) {
    trySolve(pressed);
  }
}