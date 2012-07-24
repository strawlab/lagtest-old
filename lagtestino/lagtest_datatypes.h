/* -*- Mode: C; tab-width: 4; c-basic-offset: 4 -*- */
#ifndef _LAGTEST_DATATYPES_H
#define _LAGTEST_DATATYPES_H

#include <Arduino.h>

typedef uint32_t epoch_dtype;

typedef struct {
    uint8_t value;
    epoch_dtype epoch;
    uint16_t ticks;
} timed_sample_t;

#endif
