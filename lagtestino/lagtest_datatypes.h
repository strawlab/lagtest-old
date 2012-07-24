/* -*- Mode: C; tab-width: 4; c-basic-offset: 4 -*- */
#ifndef _LAGTEST_DATATYPES_H
#define _LAGTEST_DATATYPES_H

#include <Arduino.h>

typedef uint32_t epoch_dtype;

typedef struct sample_t sample_t;

struct sample_t {
    uint8_t adc;
    epoch_dtype epoch;
    uint16_t ticks;
};

#endif
