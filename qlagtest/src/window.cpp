/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
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

//#include "glwidget.h"
#include <QWidget>
#include "window.h"

#include <QGridLayout>
#include <QLabel>
#include <QTimer>

#include "flashingbgqpaint.h"
#include "flashingbackground.h"



Window::Window(drawingType drawing, TimeModel *tm, RingBuffer<screenFlip> *screenFlips)
{
    QWidget* flipWindow;

    setWindowTitle("Lagtest - How fast is your display");
    this->resize(640, 480);

    QVBoxLayout *layout = new QVBoxLayout;
    this->msg = new QLabel("Put the light sensor ontop of the black area. When ready press the space bar.");
    this->latency = new QLabel("");

    msg->setAlignment(Qt::AlignHCenter);
    latency->setAlignment(Qt::AlignHCenter);

    layout->addWidget(msg, 0);
    layout->addWidget(latency, 0);

    if(drawing == QPAINT)
    {
        flipWindow = new flashingBGQPaint(500, tm, screenFlips);

        connect( this, SIGNAL(startMeassurement()), flipWindow, SLOT(receiveStart()) );
        connect( this, SIGNAL(stopMeassurement()), flipWindow, SLOT(receiveStop()) );

        flipWindow->resize(150, 140);
        flipWindow->show();
//        qDebug("Window max %dx%d", flipWindow->maximumWidth(), flipWindow->maximumHeight() );
//        qDebug("Size hint %dx%d", flipWindow->sizeHint().width(), flipWindow->sizeHint().height() );
//        qDebug("Size %dx%d", flipWindow->width(), flipWindow->height() );

        layout->addWidget(flipWindow, 2);

    } else if( drawing == OPENGL )
    {
        assert(0 && "Not implemented correctly");

        qDebug("Setting up a opengl window");

        // Setup OpenGL Window
//        QSurfaceFormat format;
//        format.setSamples(4);

//        FlashingBackground* fb = new FlashingBackground(500, tm, screenFlips);
//        fb->setFormat(format);
//        fb->resize(640, 480);
//        fb->show();
//        fb->setAnimating(true);

//        layout->addWidget(label, 0);
    }

    this->setLayout(layout);
}

void Window::keyPressEvent(QKeyEvent *event)
{
    //qDebug("Pressed key %c", event->key() );

    switch (event->key()) {
        case Qt::Key_Space:{
                this->msg->setText( "Remain still. Calculating Latency ..." );
                emit startMeassurement();
                emit doReset();
            break;
        }
    case Qt::Key_Q:{
        emit QCoreApplication::quit();
        }
    }

}

void Window::receiveInvalidLatency()
{
    this->msg->setText( "Adjust the position of the light sensor to be ontop of the blinking area." );
    this->latency->clear();
}

void Window::receiveUnstableLatency()
{
    this->msg->setText( "Remain still. Calculating Latency ..." );
    this->latency->clear();
}

void Window::receiveStableLatency(double latency)
{
    QString str;
    this->msg->setText( "Found a Latency of" );
    this->latency->setText( str.sprintf("%.2f ms", latency/1000000) );
}
