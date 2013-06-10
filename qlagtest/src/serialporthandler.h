#ifndef SERIALPORTHANDLER_H
#define SERIALPORTHANDLER_H

#include "timemodel.h"
#include "ringbuffer.hpp"
#include "QString"
#include "QTimer"

class LagTestSerialPortComm;

class SerialPortHandler : public QObject
{
    Q_OBJECT
public:
    explicit SerialPortHandler(QString port, int requestPeriod, TimeModel* tm, RingBuffer<clockPair>* clock_storage, RingBuffer<adcMeasurement>* adc_storage);    

signals:
    void sendDebugMsg(QString msg);
    void sendErrorMsg(QString msg);

public slots:
    void onThreadQuit();
    void start();


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

#include "timemodel.h"
#include "ringbuffer.hpp"
#include <QString>

// lagtest\firmware\lagtestino\lagtest_dataypes.h
typedef uint32_t epoch_dtype;
typedef struct {
    uint8_t cmd;    //either 'H', 'P', 'L', 'V'
    uint8_t value;
    epoch_dtype epoch;
    uint16_t ticks;
    uint8_t checksum;   //bytewise ( value + epoch + ticks ) % 256
} timed_sample_t;


class LagTestSerialPortComm : public QObject
{
    Q_OBJECT
public:
    explicit LagTestSerialPortComm(QString port, int bautRate, TimeModel* tm, RingBuffer<clockPair>* clock_storage, RingBuffer<adcMeasurement>* adc_storage);
    static int getPortIdx(QString portName);

signals:
    void finished();
    void sendDebugMsg(QString msg);
    void sendErrorMsg(QString msg);

public slots:
    void startCommunication();
    void sendClockRequest();


private:
    bool readFrame(uint8_t *buffer, timed_sample_t* frame);
    void initSerialPort();

    int write(unsigned char *data, int size);
    int read(unsigned char *buffer, int max_size);

    //Get current PC time and store data
    TimeModel* tm;
    RingBuffer<clockPair>* clock_storage;
    RingBuffer<adcMeasurement>* adc_storage;

    //Needed for handling Adruino Clock requests
    bool sendRequest;
    const static int ntimeRequests = 20;
    double timeRequests[ntimeRequests];
    unsigned int tR;

    // Serial Port Config
    //QString port;
    int bautRate;
    int portN;

};

#endif // SERIALPORTHANDLER_H
