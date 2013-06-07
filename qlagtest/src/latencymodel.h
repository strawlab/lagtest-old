#ifndef LATENCYMODEL_H
#define LATENCYMODEL_H

#include <QTimer>

#include <vector>
#include "timemodel.h"
#include "ringbuffer.hpp"


class LatencyModel : public QObject
{
     Q_OBJECT
public:
    explicit LatencyModel(int ms_updateRate, TimeModel* tm, RingBuffer<screenFlip>* screenFlips, RingBuffer<clockPair>* clock_storage, RingBuffer<adcMeasurement>* adc_storage);
    void resetHistory();

    const static int latencyHistorySize = 5;
    const static int latencyStableSize = 5;
    const static int flipHistorySize = 5;
    const static int measurementWindowSize = 100; //Take 100 adc samples before and after the screen flip
    const static int measurementHistoryLength = 10; //Keep the last 10 measurement windows and do averaging over them

    typedef uint8_t adcWindow[2][measurementHistoryLength][measurementWindowSize];

    double* getSampleTimes() { return sampleTimes; }
    adcWindow* getAdcData(){ return &adcData; }
    double getLastLatency() { return lastLatency; }
    double getAvgLatency() { return avgLatency; }
    double getAvgLatencySD() { return avgLatencySD; }
    std::vector<double> getAllLatencies() { return allLatencies; }
    double getMeasurementDuration() { return this->tm->getCurrentTime() - this->measurementStartTime; }

public slots:
    void update();
    void reset();
    void start();
    void realStart();


signals:
    void signalStableLatency();
    void signalUnstableLatency();
    void signalInvalidLatency();
    void signalUpdate(LatencyModel* lm);
    void signalNewMeassurementWindow(uint8_t* window, double* avgWindow, double* time, flip_type type);

private:
    bool findMeasurementWindow(screenFlip sf);
    double calculateLatency();
    double getProcessedLatency();
    bool isStable(int stablePeriod);
    bool detectdisplacedSensor();
    void addLatency(double latency);
    void createAvgWindow();

    QTimer* timer;
    TimeModel* tm;
    RingBuffer<clockPair>* clock;
    RingBuffer<adcMeasurement>* adc;
    RingBuffer<screenFlip>* screenFlips;

    double lastLatency;
    double avgLatency;
    double avgLatencySD;
    std::vector<double> allLatencies;
    double avgAdcWindow[2][measurementWindowSize];

    int latencyCnt;
    double latency[latencyHistorySize];

    int flipCnt;
    screenFlip flips[flipHistorySize];

    int measurementCnter[2];
    //uint8_t adcData[2][measurementHistoryLength][measurementWindowSize];
    adcWindow adcData;
    double sampleTimes[measurementWindowSize];
    long nMeasurements[2];

    double measurementStartTime;
};

#endif // LATENCYMODEL_H
