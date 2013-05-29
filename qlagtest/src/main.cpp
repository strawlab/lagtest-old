/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the documentation of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtGui/QGuiApplication>
#include <QApplication>

//#include <QtGui/QMatrix4x4>
//#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QScreen>

#include <QtCore/qmath.h>
#include "qthread.h"

#include "flashingbackground.h"
#include "timemodel.h"
#include "ringbuffer.hpp"
#include "lagtestserialportcomm.h"
#include "serialporthandler.h"
#include "latencymodel.h"
#include "flashingbgqpaint.h"

#include <QComboBox>
#include <QMainWindow>
#include <QEventLoop>
#include <QVBoxLayout>

#include "window.h"
#include "rs232.h"

std::vector<QString> discoverComPorts()
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

QString getAdruinoPort()
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

    return combo.currentText();
}

int programArduino(QString avrDudePath, QString pathToFirmware)
{
    qDebug("Trying to flash adruino ...");
    QString port = getAdruinoPort();
    char buffer[300];

    sprintf(buffer, "-F -v -pm328p -c arduino -b 115200 -P\\\\.\\%s -D -Uflash:w:%s:i", port.toStdString().c_str(), pathToFirmware.toStdString().c_str() );

    QString param(QString::fromLocal8Bit(buffer));
    qDebug("Calling %s with %s" , avrDudePath.toStdString().c_str(), param.toStdString().c_str() );
    QProcess process;
    int ret = process.execute (avrDudePath, param.split(" "));
    //process.waitForFinished(10000);
    QByteArray out = process.readAllStandardOutput();
    qDebug("Output %s" ,  out.data() );
    qDebug("Exit code %d", process.exitCode() );
    return process.exitCode();
}

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

//#define FLASH
#ifdef FLASH
    programArduino( QString::fromLocal8Bit(argv[1]), QString::fromLocal8Bit(argv[2]) );
    exit(0);
#else
    //RingBuffer<char>::test();
    //return 1;

    TimeModel tm;
    //tm.testModelGenerator();

    RingBuffer<screenFlip> screenFlips(100);
    RingBuffer<clockPair> adruinoClock(100);
    RingBuffer<adcMeasurement> adcValues(5000);

    //Make user define the serial port
    QString port = getAdruinoPort();

    qDebug("Creating handler for serial port ...");
    // Setup Serial Port Reader    
    SerialPortHandler serial(port, 500, &tm, &adruinoClock, &adcValues);
    LatencyModel lm(800, &tm, &screenFlips, &adruinoClock, &adcValues);

    qDebug("Creating main window ...");
    Window w(&tm, &screenFlips);

    QObject::connect( &w, SIGNAL(doReset()), &lm, SLOT(reset()) );
    QObject::connect( &w, SIGNAL(startMeassurement()), &serial, SLOT(start()) );
    QObject::connect( &w, SIGNAL(startMeassurement()), &lm, SLOT(start()) );


    QObject::connect( &lm, SIGNAL(signalStableLatency(double)), &w, SLOT(receiveStableLatency(double)) );
    QObject::connect( &lm, SIGNAL(signalUnstableLatency()), &w, SLOT(receiveUnstableLatency()) );
    QObject::connect( &lm, SIGNAL(signalInvalidLatency()), &w, SLOT(receiveInvalidLatency()) );
    QObject::connect( &lm, SIGNAL(signalUpdate(LatencyModel*)), &w, SLOT(receiveLatencyUpdate(LatencyModel*)) );
    QObject::connect( &lm, SIGNAL(signalNewMeassurementWindow(uint8_t*,double*,flip_type)), &w, SLOT(receiveNewMeassurementWindow(uint8_t*,double*,flip_type)) );

    w.show();
#endif
    return app.exec();
}





