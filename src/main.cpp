#include <Arduino.h>
#include <map>

#include <display.h>
#include <manual.h>
#include <puzzle_module.h>
#include <utils/button.h>

const uint8_t STAGE_LED_PINS[STAGES] = {17, 5, 18, 19, 21};
const uint8_t RED_PIN = 22, GREEN_PIN = 23;

const uint8_t BUTTON_PINS[BUTTONS] = {36, 39, 34, 35};
Button buttons[BUTTONS];

Rule **rules = NULL;

struct Stage {
  int stage_number;
  int display_number;
  int button_numbers[BUTTONS];
} current_stage;

struct StageSolution {
  int number, index;
} stage_solutions[STAGES];

void trySolve(int);

struct ButtonFunction {
  int index;
  ButtonFunction(int index) : index(index) {}
  void operator()(ButtonState state, ButtonState _) const {
    if (state != ButtonState::Pressed)
      return;
    trySolve(index);
  }
};

uint32_t random_range(uint32_t a, uint32_t b) {
  return esp_random() % (b - a + 1) + a;
}

void updateDisplayValues() {
  Display::setDisplayNumber(current_stage.display_number);
  for (int i = 0; i < BUTTONS; i++)
    Display::setButtonNumber(i, current_stage.button_numbers[i]);
}

void clearDisplayValues() {
  current_stage.display_number = 0;
  for (int i = 0; i < BUTTONS; i++)
    current_stage.button_numbers[i] = 0;
}

void resetLEDs() {
  for (int i = 0; i < STAGES; i++)
    digitalWrite(STAGE_LED_PINS[i], LOW);
}

void generateRandomStage(int stage_number) {
  Stage new_stage;
  new_stage.stage_number = stage_number;
  new_stage.display_number = random_range(1, 4);
  for (int i = 0; i < BUTTONS; i++)
    new_stage.button_numbers[i] = i + 1;
  for (int i = 0; i < BUTTONS - 1; i++) {
    int j = random_range(i, BUTTONS - 1);
    int temp = new_stage.button_numbers[i];
    new_stage.button_numbers[i] = new_stage.button_numbers[j];
    new_stage.button_numbers[j] = temp;
  }
  current_stage = new_stage;
}

void onManualCode(int code) { rules = generateRules(code); }

void start() { generateRandomStage(0); }

void restart() {
  Display::setup();
  clearDisplayValues();
  updateDisplayValues();
  resetLEDs();
}

void setup() {
  if (!PuzzleModule::setup(PuzzleModule::StatusLight(RED_PIN, GREEN_PIN)))
    ESP.restart();

  PuzzleModule::onManualCode = onManualCode;
  PuzzleModule::onStart = start;
  PuzzleModule::onRestart = restart;

  for (int i = 0; i < BUTTONS; i++) {
    buttons[i] = Button(BUTTON_PINS[i]);
    buttons[i].onStateChange = ButtonFunction(i);
  }

  for (int i = 0; i < STAGES; i++) {
    pinMode(STAGE_LED_PINS[i], OUTPUT);
    digitalWrite(STAGE_LED_PINS[i], LOW);
  }

  Display::setup();
}

void result(bool solved, int number, int index) {
  Display::prepareNextStage();
  if (solved) {
    stage_solutions[current_stage.stage_number] = {number, index};
    digitalWrite(STAGE_LED_PINS[current_stage.stage_number], HIGH);
    if (current_stage.stage_number == STAGES - 1) {
      PuzzleModule::solve();
      clearDisplayValues();
      return;
    }
    generateRandomStage(current_stage.stage_number + 1);
  } else {
    PuzzleModule::strike();
    generateRandomStage(0);
    resetLEDs();
  }
}

void check(Rule r, int number, int index) {
  if (r.rule == POSITION)
    result(r.value == index, number, index);
  else if (r.rule == LABEL)
    result(r.value + 1 == number, number, index);
  else if (r.rule == SAME_POSITION)
    result(index == stage_solutions[r.value].index, number, index);
  else if (r.rule == SAME_LABEL)
    result(number == stage_solutions[r.value].number, number, index);
}

void trySolve(int index) {
  int number = current_stage.button_numbers[index];
  Rule r = rules[current_stage.stage_number][current_stage.display_number - 1];
  check(r, number, index);
}

void loop() {
  PuzzleModule::update();
  updateDisplayValues();
  Display::update();
  if (PuzzleModule::status() != PuzzleModule::ModuleStatus::Started)
    return;
  for (int i = 0; i < BUTTONS; i++)
    buttons[i].update();
}