#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>

namespace Display {
const int DIGITS = 5;

const unsigned long STAGE_LOAD_TIME = 1000;
const unsigned long DELAY = 2;

const int SEGMENTS = 7;
const uint8_t SEGMENT_PINS[SEGMENTS] = {32, 27, 12, 26, 25, 33, 14};
const uint8_t DIGIT_PINS[DIGITS] = {13, 15, 2, 4, 16};
const bool INVERT_DIGIT[DIGITS] = {false, true, true, true, true};

const int8_t NUMBER_SEGMENTS[5] = {
    0b00000000, 0b00000110, 0b01011011, 0b01001111, 0b01100110,
};

int last_updated_digit;
int display_number;
int button_numbers[DIGITS - 1];
unsigned long next_stage_load = 0;

void setup() {
  last_updated_digit = -1;
  display_number = 0;
  for (int i = 0; i < DIGITS - 1; i++)
    button_numbers[i] = 0;

  for (int i = 0; i < 7; i++) {
    pinMode(SEGMENT_PINS[i], OUTPUT);
    digitalWrite(SEGMENT_PINS[i], LOW);
  }

  for (int i = 0; i < DIGITS; i++) {
    pinMode(DIGIT_PINS[i], OUTPUT);
    digitalWrite(DIGIT_PINS[i], LOW);
  }
}

void setDisplayNumber(int d) { display_number = d; }

void setButtonNumber(int p, int d) { button_numbers[p] = d; }

void setDigit(int digit, bool state, bool invert) {
  digitalWrite(DIGIT_PINS[digit], state ^ invert);
}

void setSegments(int number, bool invert) {
  for (int i = 0; i < SEGMENTS; i++) {
    bool result = ((~NUMBER_SEGMENTS[number]) & (1 << i));
    digitalWrite(SEGMENT_PINS[i], invert ? !result : result);
  }
}

void prepareNextStage() {
  next_stage_load = millis() + STAGE_LOAD_TIME;
  for (int i = 0; i < DIGITS; i++)
    setDigit(i, false, INVERT_DIGIT[i]);
}

void update() {
  unsigned long now = millis();
  if (next_stage_load > now)
    return;
  int digit = ((now - next_stage_load) / DELAY) % DIGITS;
  if (digit == last_updated_digit)
    return;
  int previous_digit = (digit - 1 + DIGITS) % DIGITS;
  setDigit(previous_digit, false, INVERT_DIGIT[previous_digit]);
  int number = (digit == 0) ? display_number : button_numbers[digit - 1];
  setSegments(number, INVERT_DIGIT[digit]);
  setDigit(digit, true, INVERT_DIGIT[digit]);
}
} // namespace Display

#endif // DISPLAY_H