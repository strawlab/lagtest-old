#ifndef LATENCYMODEL_H
#define LATENCYMODEL_H

#include "timemodel.h"
#include "ringbuffer.hpp"

class LatencyModel : public QObject
{
     Q_OBJECT
public:
    LatencyModel(TimeModel* tm, RingBuffer<clockPair>* clock_storage, RingBuffer<adcMeasurement>* adc_storage);

public slots:
    void update();

private:
    adcMeasurement findFlipp();
};

#endif // LATENCYMODEL_H
