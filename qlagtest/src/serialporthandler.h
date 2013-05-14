#ifndef SERIALPORTHANDLER_H
#define SERIALPORTHANDLER_H

#include "timemodel.h"
#include "ringbuffer.hpp"
#include "lagtestserialportcomm.h"
#include "QString"
#include "QTimer"

class SerialPortHandler : public QObject
{
    Q_OBJECT
public:
    explicit SerialPortHandler(QString port, int requestPeriod, TimeModel* tm, RingBuffer<clockPair>* clock_storage, RingBuffer<adcMeasurement>* adc_storage);
    void start();

public slots:
    void onThreadQuit();


private:
    QTimer* timer;
    QThread* thread;
    LagTestSerialPortComm* serial;

    TimeModel* tm;
    RingBuffer<clockPair>* clock_storage;
    RingBuffer<adcMeasurement>* adc_storage;

    QString port;
    int requestPeriod;
    void createSerialPortCommunicator();

};

#endif // SERIALPORTHANDLER_H
