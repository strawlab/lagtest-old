#ifndef LATENCYMODEL_H
#define LATENCYMODEL_H

#include <QTimer>

#include "timemodel.h"
#include "ringbuffer.hpp"


class LatencyModel : public QObject
{
     Q_OBJECT
public:
    LatencyModel(int ms_updateRate, TimeModel* tm, RingBuffer<double>* screenFlips, RingBuffer<clockPair>* clock_storage, RingBuffer<adcMeasurement>* adc_storage);

public slots:
    void update();

private:
    bool findFlipp(adcMeasurement *flip);
    double calculateLatency(adcMeasurement adc);

    QTimer timer;
    TimeModel* tm;
    RingBuffer<clockPair>* clock;
    RingBuffer<adcMeasurement>* adc;
    RingBuffer<double>* screenFlips;

    const static int latencyHistorySize = 10;
    int latencyCnt;
    double latency[latencyHistorySize];

    const static int flipHistorySize = 5;
    int flipCnt;
    double flips[flipHistorySize];
};

#endif // LATENCYMODEL_H
