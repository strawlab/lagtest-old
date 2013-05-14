#include "lagtestserialportcomm.h"

#include <QtCore/QDebug>
#include <exception>
#include <assert.h>

#include <QCoreApplication>

LagTestSerialPortComm::LagTestSerialPortComm(QString port, int bautRate,TimeModel* tm, RingBuffer<clockPair>* clock_storage, RingBuffer<adcMeasurement>* adc_storage) :
    QObject(0),
    tm(tm),
    clock_storage(clock_storage),
    adc_storage(adc_storage),
    sendRequest(false),
    bautRate(bautRate),
    port(port),
    tR(0)
{    

}

void LagTestSerialPortComm::initSerialPort()
{
    qDebug("Starting LagTest Serial Communicator ... ");
    //QSerialPort serial;
    this->serial = new QSerialPort;
    serial->setPortName(this->port);

    if( !serial->open(QIODevice::ReadWrite) ){
        qCritical("Opening serial on %s failed!", port.toStdString().c_str() );
        throw std::exception();
    }

    if( !(serial->setBaudRate( this->bautRate ) &&  serial->setDataBits(QSerialPort::Data8) && serial->setParity(QSerialPort::NoParity) && serial->setStopBits(QSerialPort::OneStop) ) ){
        qCritical("Setting serial port configuration failed!", port.toStdString().c_str() );
        throw std::exception();
    }
}



void LagTestSerialPortComm::sendClockRequest()
{
    //qDebug("Received signal for clock request");
    this->sendRequest = true;
}

void LagTestSerialPortComm::startCommunication()
{    
    qint64 frameLength = 9;
    const static int bufferSize = 100;

    clockPair cp;
    adcMeasurement adcM;

    uint8_t buffer[ bufferSize ];
    timed_sample_t frame;
    int i;
    int nBuffer;
    bool validFrame = false;
    int nBytes;

    char b[2];

    try{
        this->initSerialPort();
    } catch (exception &e) {
        qCritical("Opening Serial Port failed!");
        emit finished();
    }

    nBuffer = 0;  //Number of unread bytes in the buffer
    qDebug("Adruino Frame length = %d bytes", frameLength );

    while(1)
    {


        if( this->sendRequest )
        {
            b[0] = 'P';
            b[1] = this->tR;

            qDebug("Sending request for Adruino Clock time ...");
            this->serial->write(b, 2);
            //this->serial->waitForBytesWritten(1000);

            this->timeRequests[tR] = this->tm->getCurrentTime();
            tR = (tR+1)%this->ntimeRequests;

            this->sendRequest = false;
        }
        this->serial->clear();
QCoreApplication::processEvents();  //If this is not called, no signals can be received by this thread
        continue;
        //qDebug("%d bytes in buffer", nBuffer);
        while( nBuffer < frameLength )
        {
            this->serial->waitForReadyRead(1000);

            assert ( nBuffer < bufferSize );
            nBytes = serial->read((char*)&(buffer[nBuffer]), (bufferSize-nBuffer) );
            qDebug("Read %d bytes from serial", nBytes);
            nBuffer += nBytes;           

            //qDebug("Processing events");
            QCoreApplication::processEvents();  //If this is not called, no signals can be received by this thread
        }


        //Try to read a new frame from the beginning of the buffer
        //If the frame checksum fails, do a frame shift and try again

        validFrame = false;
        i = 0;
        //do frame shifts untill there cannot be a full frame in the buffer
        while( !validFrame && (i+frameLength) <= nBuffer )
        {
            validFrame = readFrame(&buffer[i], &frame);

            if( validFrame ) {
               i += frameLength;
            } else {
                i++; //Shift the frame by one
            }
        }
        // i ... number of bytes read/garbage in buffer

        //Move read/garbage data to the beginning of the buffer
        assert ( i <= nBuffer );
        for(int j = 0; j < (nBuffer-i) ; j++){
            buffer[j] = buffer[j+i];
        }
        nBuffer -= i;
        assert( nBuffer >= 0 );

        if( validFrame )
        {
            qDebug( " Frame: %c;%d;%d;%d", frame.cmd, frame.value, frame.epoch, frame.ticks );

            switch(frame.cmd)
            {
                case 'H'://ADC measurement
                {
                    adcM.adc = frame.value;
                    adcM.adruino_epoch = frame.epoch;
                    adcM.adruino_ticks = frame.ticks;
                    this->adc_storage->put( &adcM );
                    //qDebug("Received a adc measurement");
                    break;
                }
                case 'P': //Clock response
                {
                    cp.local = this->tm->getCurrentTime();
                    cp.adruino_epoch = frame.epoch;
                    cp.adruino_ticks = frame.ticks;
                    this->clock_storage->put( &cp );
                    qDebug("Received a adruino clock msg for request %d ", frame.value );
                    break;
                }
                default:{
                    qDebug( "Unknown msg from adruino: %c;%d;%d;%d", frame.cmd, frame.value, frame.epoch, frame.ticks );
                }
            }



        } else {
            qDebug( " Invalid Frame ... ");
        }


    }

}


bool LagTestSerialPortComm::readFrame(uint8_t* buffer, timed_sample_t* frame)
{
    //Check if the checksum is valid and if so, fill the data structure
    uint8_t cs;
    cs = *(buffer+1)+ *(buffer+2)+ *(buffer+3)+ *(buffer+4)+ *(buffer+5) + *(buffer+6) + *(buffer+7);;
//    for(int i=0; i < 6; i++){
//        cs += *(buffer+1 + i);
//    }
    if( cs == *(buffer+8))
    {
        frame->cmd =    *(buffer+0);
        frame->value =  *(buffer+1);
        frame->epoch = (*(buffer+5) << 24) + (*(buffer+4) << 16) + (*(buffer+3) << 8) + *(buffer+2);
        frame->ticks = (*(buffer+7) << 8) + *(buffer+6);
        frame->checksum = *(buffer+8);
        return true;
    } else{
        qDebug("Invalid Frame (%u) (%u) (%u;%u;%u;%u) (%u;%u;) (%u) | CS %u", *(buffer+0), *(buffer+1), *(buffer+2), *(buffer+3), *(buffer+4), *(buffer+5), *(buffer+6), *(buffer+7), *(buffer+8) , cs);
        return false;
    }
}
