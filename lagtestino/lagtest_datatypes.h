/* -*- Mode: C; tab-width: 4; c-basic-offset: 4 -*- */
#ifndef _LAGTEST_DATATYPES_H
#define _LAGTEST_DATATYPES_H

#include <Arduino.h>

typedef uint32_t epoch_dtype;

typedef struct {
    uint8_t adc;
    epoch_dtype epoch;
    uint16_t ticks;
} sample_t;

#endif
