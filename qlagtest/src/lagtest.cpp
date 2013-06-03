#include "lagtest.h"

#include <QSettings>
#include <QString>
#include <QComboBox>
#include <QVBoxLayout>
#include <QEventLoop>
#include <QProcess>

#include "timemodel.h"
#include "latencymodel.h"
#include "serialporthandler.h"
#include "window.h"
#include "rs232.h"

LagTest::LagTest(int clockSyncPeriod, int latencyUpdate, int screenFlipPeriod)
{
    TimeModel* tm = new TimeModel();
    //tm.testModelGenerator();

    RingBuffer<screenFlip>* screenFlips = new RingBuffer<screenFlip>(100);
    RingBuffer<clockPair>* adruinoClock = new RingBuffer<clockPair>(100);
    RingBuffer<adcMeasurement>* adcValues = new RingBuffer<adcMeasurement>(5000);

    this->loadSettings();

    //Make user define the serial port
    QString port = settings->value("Adruino/Port").toString();

    qDebug("Creating handler for serial port on %s...", port.toStdString().c_str() );
    // Setup Serial Port Reader
    SerialPortHandler* serial = new SerialPortHandler(port, clockSyncPeriod, tm, adruinoClock, adcValues);
    LatencyModel* lm = new LatencyModel(latencyUpdate, tm, screenFlips, adruinoClock, adcValues);

    Window* w = new Window(tm, screenFlips);

    QObject::connect( w, SIGNAL(doReset()), lm, SLOT(reset()) );
    QObject::connect( w, SIGNAL(startMeassurement()), serial, SLOT(start()) );
    QObject::connect( w, SIGNAL(startMeassurement()), lm, SLOT(start()) );

    //QObject::connect( &w, &Window::flashAdruino, getAdruinoPort );


    QObject::connect( lm, SIGNAL(signalStableLatency()),        w, SLOT(receiveStableLatency()) );
    QObject::connect( lm, SIGNAL(signalUnstableLatency()),      w, SLOT(receiveUnstableLatency()) );
    QObject::connect( lm, SIGNAL(signalInvalidLatency()),       w, SLOT(receiveInvalidLatency()) );
    QObject::connect( lm, SIGNAL(signalUpdate(LatencyModel*)),  w, SLOT(receiveLatencyUpdate(LatencyModel*)) );
    QObject::connect( lm, SIGNAL(signalNewMeassurementWindow(uint8_t*,double*,flip_type)), w, SLOT(receiveNewMeassurementWindow(uint8_t*,double*,flip_type)) );

    w->show();
}

std::vector<QString> LagTest::discoverComPorts()
{
    std::vector<int> ports;
    std::vector<QString> portsNames;
    char cbuffer[10];

    qDebug("Discovering Serial Ports ...");

    for(int i = 5; i < 16; i++){    //Only search for ports between COM6-COM16
        if( !RS232_OpenComport(i, 9600) ){
            ports.push_back(i);
            RS232_CloseComport( i );
        }
    }

    #ifdef __linux__
    assert(0 && "No linux implementation");
    #else
        for(std::vector<int>::iterator it = ports.begin(); it != ports.end(); ++it) {
            sprintf(cbuffer, "COM%d", (*it)+1);
            portsNames.push_back( QString( cbuffer ) );
        }
    #endif

    return portsNames;
}

void LagTest::receiveFlashAdruino()
{
    this->settings->beginGroup("Adruino");
    settings->value("port");
}

QString LagTest::makeUserSelectPort()
{
    //Create a simple QWidget that contains a dropdown list of all serial ports
    QWidget mainWindow;
    QComboBox combo;
    QVBoxLayout layout;
    QEventLoop loop;
    QLabel PortSelectMsg("Select the port adruino is connected to");

    PortSelectMsg.setAlignment(Qt::AlignHCenter);
    layout.addWidget( &PortSelectMsg, 0 );
    layout.addWidget( &combo, 0 );
    mainWindow.setLayout( &layout );

    std::vector<QString> ports = discoverComPorts();
    for(std::vector<QString>::iterator it = ports.begin(); it != ports.end(); ++it) {
        //qDebug("Found Port %s" , (*it).toStdString().c_str() );
        combo.addItem(*it);
    }
    QObject::connect(&combo, SIGNAL(activated(QString)), &loop, SLOT( quit() ));

    mainWindow.show();
    loop.exec();    //Wait till the user selects an entry
    mainWindow.close();

    qWarning( "User selected Port %s " , combo.currentText().toStdString().c_str() );
    return combo.currentText();
}

int LagTest::programArduino(QString avrDudePath, QString pathToFirmware, QString port)
{
    qDebug("Trying to flash adruino ...");
    char buffer[300];
    if( port.isEmpty() ){
        port = makeUserSelectPort();
    }

    sprintf(buffer, "-F -v -pm328p -c arduino -b 115200 -P\\\\.\\%s -D -Uflash:w:%s:i", port.toStdString().c_str(), pathToFirmware.toStdString().c_str() );

    QString param(QString::fromLocal8Bit(buffer));
    qDebug("Calling %s with %s" , avrDudePath.toStdString().c_str(), param.toStdString().c_str() );

    QProcess process;
    int ret = process.execute (avrDudePath, param.split(" "));

    QByteArray out = process.readAllStandardOutput();
    qDebug("Output %s" ,  out.data() );
    qDebug("Exit code %d", process.exitCode() );

    return process.exitCode();
}

bool LagTest::loadSettings()
{
    this->settings = new QSettings("lagtest.ini", QSettings::IniFormat);

    //If we dont have this setting, or the port is not valid, let the user define a new port
    if( !this->testPort( settings->value("Adruino/Port").toString() ))
    {
        qDebug("Port not valid, query user to specify new one.");
        QString nPort;
        do{
            nPort = makeUserSelectPort();
            if( !this->testPort(nPort) ){
                qCritical("Invalid Port!");
                nPort = "";
            }
        } while( nPort.isEmpty() );
        settings->setValue("Adruino/Port", nPort );
        settings->sync();
    }

    return true;

    //Create entries
    //    settings->beginGroup("Adruino");
    //        settings->setValue("Port", "COM11");
    //        settings->setValue("avrDudePath", "tools/avrdude.exe");
    //        settings->setValue("firmwarePath", "firmware.hex");
    //    settings->endGroup();
    //    settings->sync();
}

bool LagTest::testPort(QString port)
{
    int portIdx = LagTestSerialPortComm::getPortIdx(port);

    if( portIdx < 0)
        return false;

    if( !RS232_OpenComport(portIdx, 9600) ){
        RS232_CloseComport( portIdx );
        return true;
    }
    return false;
}
