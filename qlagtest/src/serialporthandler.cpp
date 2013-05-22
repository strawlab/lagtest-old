#include "serialporthandler.h"

#include <QThread>

SerialPortHandler::SerialPortHandler(QString port, int requestPeriod, TimeModel *tm, RingBuffer<clockPair> *clock_storage, RingBuffer<adcMeasurement> *adc_storage) :
    QObject(0),
    thread(NULL),
    port(port),
    requestPeriod(requestPeriod),
    tm(tm),
    clock_storage(clock_storage),
    adc_storage(adc_storage)
{
    //Start a thread and execute doProcessing()
    this->createSerialPortCommunicator();
}

void SerialPortHandler::createSerialPortCommunicator()
{
    try{
        this->serial = new LagTestSerialPortComm( this->port, 115200, this->tm, this->clock_storage, this->adc_storage );

        // Setup period query for adruino clock
        this->timer = new QTimer(this);
        connect(timer, SIGNAL(timeout()), serial, SLOT(sendClockRequest()));
        timer->start(requestPeriod);

        this->thread = new QThread();
        serial->moveToThread(thread);
        connect(thread, SIGNAL(started()), serial, SLOT(startCommunication()));
        connect(serial, SIGNAL(finished()), this, SLOT(onThreadQuit()));
        connect(serial, SIGNAL(finished()), serial, SLOT(deleteLater()));

    } catch (exception &e){
        qCritical("Connecting to the lagtest serial board failed!");
        throw e;
    }
}

void SerialPortHandler::onThreadQuit()
{
    qDebug("Main: Serial Communicator finished ..." );

    qDebug("Will restart serial port Communicator!");

    try
    {
        delete this->serial;
        delete this->thread;
        delete this->timer;

        this->createSerialPortCommunicator();
    } catch (exception &e){
        qCritical("Restarting Serial Port Communicator failed!");
        throw e;
    }
}

void SerialPortHandler::start()
{
    qDebug("Starting serial port reader thread ...");
    if(this->thread)
        this->thread->start();
}
