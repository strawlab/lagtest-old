/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

const uint8_t OUT_CHANNEL = 3;
const uint8_t IN_CHANNEL = 4;

void setup() {
    pinMode(OUT_CHANNEL, OUTPUT);
    pinMode(IN_CHANNEL, INPUT);
}

// global variable
int prev_value=2; // impossible value

void loop() {
    int value = digitalRead(IN_CHANNEL);
    if (value!=prev_value) {
        // changed value, sleep
        delay(100); // add 100 msec

        if (value==HIGH) {
            digitalWrite(OUT_CHANNEL, LOW);
        } else {
            digitalWrite(OUT_CHANNEL, HIGH);
        }

        prev_value = value;
    }
}
