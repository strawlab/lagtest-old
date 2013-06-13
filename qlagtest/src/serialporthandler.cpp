#include "serialporthandler.h"

#include <QThread>
#include <QCoreApplication>

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

        connect(serial, SIGNAL(sendDebugMsg(QString)), this, SIGNAL(sendDebugMsg(QString)) );
        connect(serial, SIGNAL(sendErrorMsg(QString)), this, SIGNAL(sendErrorMsg(QString)) );


    } catch (exception &e){
        //qCritical("Connecting to the lagtest serial board failed!");
        this->sendErrorMsg("Connecting to the lagtest serial board failed!");
        throw e;
    }
}

void SerialPortHandler::onThreadQuit()
{
    //qDebug("Main: Serial Communicator finished ..." );
    //qDebug("Will restart serial port Communicator!");
    this->sendDebugMsg("Will restart serial port Communicator!");

    try
    {
        delete this->serial;
        delete this->thread;
        delete this->timer;

        this->createSerialPortCommunicator();
    } catch (exception &e){
        //qCritical("Restarting Serial Port Communicator failed!");
        this->sendErrorMsg("Restarting Serial Port Communicator failed!");
        throw e;
    }
}

void SerialPortHandler::start()
{
    //qDebug("Starting serial port reader thread ...");
    this->sendDebugMsg("Will start serial port Communicator!");

    if(this->thread)
        this->thread->start();
}

#define _CRT_SECURE_NO_WARNINGS
#include "rs232.h"

LagTestSerialPortComm::LagTestSerialPortComm(QString port, int bautRate,TimeModel* tm, RingBuffer<clockPair>* clock_storage, RingBuffer<adcMeasurement>* adc_storage) :
    QObject(0),
    tm(tm),
    clock_storage(clock_storage),
    adc_storage(adc_storage),
    sendRequest(false),
    bautRate(bautRate),    
    tR(0)
{
    this->portN = this->getPortIdx(port);
    if( portN < 0)
    {
        //qCritical("Invalid Com port [%s]!", port.toStdString().c_str() );
        QString s;
        this->sendErrorMsg( s.sprintf("Invalid Com port [%s]!", port.toStdString().c_str()));
        throw std::exception();
    }

}

int LagTestSerialPortComm::getPortIdx(QString portName)
{
    int idx = -1;  

    if( portName.length() >= 4)
    {
        if( portName.contains("Com", Qt::CaseInsensitive)){
            //qDebug("Detect COMXX");
            idx = portName.right(portName.length()-3).toInt() - 1; //From Com11 , extract 11, convert it to int, rs232 starts counting from 0.
        } else if( portName.contains("USB", Qt::CaseInsensitive) ){
            //qDebug("Detect USBXX");
            idx = portName.right(portName.length()-3).toInt() + 16; //From USB0 , extract 0, convert it to int, rs232 starts counting from 16.
        } else {            
            //qWarning("Invalid port name!");
            fprintf(stderr, "Invalid port name!");
        }
    }
    return idx;
}

void LagTestSerialPortComm::initSerialPort()
{    
    QString s;
    this->sendDebugMsg(s.sprintf("Opening Com port %d ... ", portN+1));
    if(RS232_OpenComport(portN, this->bautRate))
    {        
        this->sendErrorMsg("Can not open Serial port.");
        throw std::exception();
    }    
}

int LagTestSerialPortComm::write(unsigned char* data, int size){
    return RS232_SendBuf(this->portN, data, size);
}

int  LagTestSerialPortComm::read(unsigned char* buffer, int max_size){
    return RS232_PollComport(this->portN, buffer, max_size);
}

void LagTestSerialPortComm::sendClockRequest()
{
    //sendDebugMsg("Received signal to send Time Request ...");
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
    double sendTime, now, d1;
    unsigned char b[2];
    int nEmptyReads;

    try{
        this->initSerialPort();
    } catch( ... ) {        
        this->sendErrorMsg("Opening Serial Port failed!");
        emit finished();
    }

    nBuffer = 0;  //Number of unread bytes in the buffer
    //qDebug("Adruino Frame length = %lld bytes", frameLength );

    while(1)
    {        
        QCoreApplication::processEvents();  //If this is not called, no signals can be received by this thread

        //If ordered, send an request to the adruino to return its current clock value
        if( this->sendRequest )
        {
            b[0] = 'P';
            b[1] = this->tR;

            //sendDebugMsg("Sending request for Adruino Clock time ...");
            this->write(b, 2);

            this->timeRequests[tR] = this->tm->getCurrentTime();
            tR = (tR+1)%this->ntimeRequests;

            this->sendRequest = false;                       
        }

        nEmptyReads = 0;
        //Read from the serial port at least one complete message
        while( nBuffer < frameLength )
        {            
            nBytes = this->read(&(buffer[nBuffer]), (bufferSize-nBuffer) );            
            nBuffer += nBytes;
            if(nBytes == 0){
                nEmptyReads ++;
                QThread::msleep ( 10 ) ;                
            }

            if(nEmptyReads > 100){
                this->sendErrorMsg("Not getting any data from the arduino!");
                break;
            }
        }
        //qDebug("Buffer at %d", nBuffer);
        assert ( nBuffer <= bufferSize );

        if( nBuffer >= frameLength )
        {
            //Try to read a new frame from the beginning of the buffer
            //If the frame checksum fails, do a frame shift and try again
            validFrame = false;
            i = 0;
            //do frame shifts untill there cannot be a full frame in the buffer
            while( !validFrame && (i+frameLength) <= nBuffer )
            {
                validFrame = readFrame(&buffer[i], &frame);

                if( validFrame ) {
                   now = this->tm->getCurrentTime();
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
                //qDebug( " Frame: %c;%d;%d;%d", frame.cmd, frame.value, frame.epoch, frame.ticks );

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
                    //Get the time the request was send; assume the latency of receiving the reply is symetrical; store the time on this pc and the adruino clock
                        if( frame.value > this->ntimeRequests ){
                            //qCritical("Adruino returns invalid reference id!");
                            this->sendErrorMsg("Adruino returns invalid reference id!");
                        } else {
                            sendTime = this->timeRequests[ frame.value ];
                            d1 = (now - sendTime)/2.0;
                            if( d1 <= 0){
                                //qCritical("somethign strange happens here ... %g", d1 );
                                QString s;
                                this->sendErrorMsg(s.sprintf("somethign strange happens here ... %g", d1 ));
                                d1 = 0;
                            }
                            cp.local = sendTime + d1;
                            cp.adruino_epoch = frame.epoch;
                            cp.adruino_ticks = frame.ticks;
                            this->clock_storage->put( &cp );
                            //qDebug("Received a adruino clock msg for request %d from %g", frame.value, cp.local );
                            break;
                        }
                    }
                    default:{
                        //qDebug( "Unknown msg from adruino: %c;%d;%d;%d", frame.cmd, frame.value, frame.epoch, frame.ticks );
                    }
                }
            } else {
                //qDebug( " Invalid Frame ... ");
                this->sendErrorMsg("Invalid Frame ... ");
            }
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
        //qDebug("Invalid Frame (%u) (%u) (%u;%u;%u;%u) (%u;%u;) (%u) | CS %u", *(buffer+0), *(buffer+1), *(buffer+2), *(buffer+3), *(buffer+4), *(buffer+5), *(buffer+6), *(buffer+7), *(buffer+8) , cs);
        return false;
    }
}
