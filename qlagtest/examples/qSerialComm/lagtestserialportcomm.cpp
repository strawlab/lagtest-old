#include "lagtestserialportcomm.h"

#include <QtCore/QDebug>

#include <assert.h>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

LagTestSerialPortComm::LagTestSerialPortComm(int bautRate) :
    QObject(0)
{    
    this->bautRate = bautRate;
}


void LagTestSerialPortComm::startCommunication()
{
    qDebug("Starting LagTest Serial Communicator ... ");
    QSerialPort serial;

    serial.setPortName("Com11");

    if( !serial.open(QIODevice::ReadWrite) )
        emit finished();

    if( !(serial.setBaudRate( this->bautRate ) &&  serial.setDataBits(QSerialPort::Data8) && serial.setParity(QSerialPort::NoParity) && serial.setStopBits(QSerialPort::OneStop) ) )
        emit finished();

    qint64 frameLength = 9;
    const static int bufferSize = 100;

    uint8_t buffer[ bufferSize ];
    timed_sample_t frame;
    int i;
    int nBuffer;
    bool validFrame = false;
    int nBytes;

    nBuffer = 0;  //Number of unread bytes in the buffer
    qDebug("Frame length = %d bytes", frameLength );
    while(1)
    {
        qDebug("%d bytes in buffer", nBuffer);
        while( nBuffer < frameLength )
        {
            serial.waitForReadyRead(2000);

            assert ( nBuffer < bufferSize );
            nBytes = serial.read((char*)&(buffer[nBuffer]), (bufferSize-nBuffer) );
            qDebug("Read %d bytes from serial", nBytes);
            nBuffer += nBytes;
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
