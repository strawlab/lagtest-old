#include "mainapp.h"
#include "lagtestserialportcomm.h"

#include "qdebug.h"
#include "qthread.h"

mainApp::mainApp(QObject *parent) :
    QObject(parent)
{    
}

void mainApp::start()
{
    //Start a thread and execute doProcessing()
    QThread* thread = new QThread();
    LagTestSerialPortComm* serial = new LagTestSerialPortComm( 115200 );

    serial->moveToThread(thread);
    //connect(worker, SIGNAL(error(QString)), this, SLOT(errorString(QString)));
    connect(thread, SIGNAL(started()), serial, SLOT(startCommunication()));
    connect(serial, SIGNAL(finished()), this, SLOT(onThreadQuit()));
    connect(serial, SIGNAL(finished()), serial, SLOT(deleteLater()));
    //connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    thread->start();

}



void mainApp::onThreadQuit()
{
    qDebug("Main: Serial Communicator finished ..." );
}
