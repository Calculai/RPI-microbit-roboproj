#include <Arduino.h>

const uint8_t rowPins[] = {21, 22, 23, 24, 25};
const uint8_t columnPins[] = {4, 7, 3, 6, 10};
const uint8_t FRAME_COUNT = 16;
const uint8_t frames[FRAME_COUNT][5][5] = {
    {
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
    },
    {
        {0, 0, 0, 1, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 1, 0, 0, 0},
        {0, 1, 0, 0, 0},
    },
    {
        {0, 0, 0, 0, 1},
        {0, 0, 0, 1, 0},
        {0, 0, 1, 0, 0},
        {0, 1, 0, 0, 0},
        {1, 0, 0, 0, 0},
    },
    {
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 1},
        {0, 1, 1, 1, 0},
        {1, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
    },
    {
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {1, 1, 1, 1, 1},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
    },
    {
        {0, 0, 0, 0, 0},
        {1, 0, 0, 0, 0},
        {0, 1, 1, 1, 0},
        {0, 0, 0, 0, 1},
        {0, 0, 0, 0, 0},
    },
    {
        {1, 0, 0, 0, 0},
        {0, 1, 0, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 0, 1, 0},
        {0, 0, 0, 0, 1},
    },
    {
        {0, 1, 0, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 0, 1, 0},
    },
    {
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
    },
    {
        {0, 0, 0, 1, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 1, 0, 0, 0},
        {0, 1, 0, 0, 0},
    },
    {
        {0, 0, 0, 0, 1},
        {0, 0, 0, 1, 0},
        {0, 0, 1, 0, 0},
        {0, 1, 0, 0, 0},
        {1, 0, 0, 0, 0},
    },
    {
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 1},
        {0, 1, 1, 1, 0},
        {1, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
    },
    {
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {1, 1, 1, 1, 1},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
    },
    {
        {0, 0, 0, 0, 0},
        {1, 0, 0, 0, 0},
        {0, 1, 1, 1, 0},
        {0, 0, 0, 0, 1},
        {0, 0, 0, 0, 0},
    },
    {
        {1, 0, 0, 0, 0},
        {0, 1, 0, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 0, 1, 0},
        {0, 0, 0, 0, 1},
    },
    {
        {0, 1, 0, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 0, 1, 0},
    },
};
unsigned long previous_rot = 0;
int iterator = 0;


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
    unsigned long now = millis(); 
    unsigned long update_cadence = 100; // adjustable update cadence (spin speed)

    if (now - previous_rot >= update_cadence) {
        previous_rot = now;
        iterator = (iterator + 1) % FRAME_COUNT;
    }
    
    for (uint8_t row = 0; row < 5; row++) {
        for (uint8_t column = 0; column < 5; column++) {
            digitalWrite(columnPins[column], frames[iterator][row][column] ? LOW : HIGH);
        }
        digitalWrite(rowPins[row], HIGH);
        delay(2);
        digitalWrite(rowPins[row], LOW);
    }
}