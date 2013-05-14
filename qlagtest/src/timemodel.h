#ifndef TIMEMODEL_H
#define TIMEMODEL_H

#include <QElapsedTimer>
#include <stdint.h>

typedef struct{
    uint16_t adruino_ticks;
    uint32_t adruino_epoch;
    double local;
} clockPair;

typedef struct{
    uint16_t adruino_ticks;
    uint32_t adruino_epoch;
    uint8_t adc;
} adcMeasurement;

class TimeModel
{
public:
    TimeModel();

    void testModelGenerator();
    double getCurrentTime();

private:
    QElapsedTimer*  timer;
};

#endif // TIMEMODEL_H
