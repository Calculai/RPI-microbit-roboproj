#include <Arduino.h>

const uint8_t rowPins[] = {21, 22, 23, 24, 25};
const uint8_t columnPins[] = {4, 7, 3, 6, 10};
const uint8_t display[5][5] = {
    {1, 0, 1, 0, 1},
    {0, 1, 1, 1, 0},
    {1, 1, 1, 1, 1},
    {0, 1, 0, 1, 0},
    {1, 0, 0, 0, 1},
};

void setup() {
    for (uint8_t row = 0; row < 5; row++) {
        pinMode(rowPins[row], OUTPUT);
        digitalWrite(rowPins[row], LOW);
    }
    for (uint8_t column = 0; column < 5; column++) {
        pinMode(columnPins[column], OUTPUT);
        digitalWrite(columnPins[column], HIGH);
    }
}

void loop() {
    for (uint8_t row = 0; row < 5; row++) {
        for (uint8_t column = 0; column < 5; column++) {
            digitalWrite(columnPins[column], display[row][column] ? LOW : HIGH);
        }
        digitalWrite(rowPins[row], HIGH);
        delay(2);
        digitalWrite(rowPins[row], LOW);
    }
}