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

#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_point_data.h>

#include "latencymodel.h"

//Window::Window(enum drawingType drawing, TimeModel *tm, RingBuffer<screenFlip> *screenFlips)
Window::Window(TimeModel *tm, RingBuffer<screenFlip> *screenFlips) :
    showPlot(false), updateCurveIdx(0)
{
    QWidget* flipWindow;
	enum drawingType drawing = Window::QPAINT;
    setWindowTitle("Lagtest - How fast is your display");
    this->resize(640, 480);

    QVBoxLayout *layout = new QVBoxLayout;
    this->msg = new QLabel("Put the light sensor ontop of the white area. When ready press the space bar.");
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


    this->xData = (double*) malloc(sizeof(double) * LatencyModel::measurementWindowSize );
    this->yData = (double*) malloc(sizeof(double) * LatencyModel::measurementWindowSize);
//    this->yData = (double**) malloc(sizeof(double*) * LatencyModel::measurementHistoryLength);
//    for(int i=0; i < LatencyModel::measurementHistoryLength; i++){
//        yData[i] = (double*) malloc(sizeof(double)* LatencyModel::measurementWindowSize );
//    }

    this->plot = new SubWindow(this);
    QVBoxLayout *plotLayout = new QVBoxLayout;

    this->cPlots[BLACK_TO_WHITE] = new QwtPlot( QwtText("Black to White") );
    this->cPlots[WHITE_TO_BLACK] = new QwtPlot( QwtText("White to Black") );

    cPlots[BLACK_TO_WHITE]->setCanvasBackground(QBrush(Qt::white));
    cPlots[WHITE_TO_BLACK]->setCanvasBackground(QBrush(Qt::white));

    this->nCurves = LatencyModel::measurementHistoryLength;
//    this->curves[BLACK_TO_WHITE] = (QwtPlotCurve*) malloc (sizeof(QwtPlotCurve*) * nCurves);
//    this->curves[WHITE_TO_BLACK] = (QwtPlotCurve*) malloc (sizeof(QwtPlotCurve*) * nCurves);

    int colors[] = { Qt::red, Qt::blue, Qt::yellow, Qt::green, Qt::gray, Qt::darkGray, Qt::darkBlue, };
    int nColors = sizeof(colors);

    QPen p;
    p.setWidth( 1 );

    for(int i=0; i < nCurves; i++)
    {
        p.setColor( QColor( (Qt::GlobalColor) colors[i%nColors] ) );

        curves[BLACK_TO_WHITE].push_back( new QwtPlotCurve() );
        curves[WHITE_TO_BLACK].push_back( new QwtPlotCurve() );

        curves[BLACK_TO_WHITE][i]->setPen( p );
        curves[WHITE_TO_BLACK][i]->setPen( p );

        double x[] = { 0.0, 10.0, 20.0 };
        double y[] = { 0.0, 0.0, 0.0 };
        QwtPointArrayData *data = new QwtPointArrayData(x, y, 3);

        curves[BLACK_TO_WHITE][i]->setData( data );
        curves[WHITE_TO_BLACK][i]->setData( data );

        curves[BLACK_TO_WHITE][i]->attach( cPlots[BLACK_TO_WHITE] );
        curves[WHITE_TO_BLACK][i]->attach( cPlots[WHITE_TO_BLACK] );
    }

    plotLayout->addWidget( cPlots[BLACK_TO_WHITE] );
    plotLayout->addWidget( cPlots[WHITE_TO_BLACK] );

    plot->setLayout( plotLayout );

    if( this->showPlot ) {
        this->plot->show();
    } else {
        this->plot->hide();
    }
    showPlot = !showPlot;

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
    case Qt::Key_D:{
            if( this->showPlot ) {
                this->plot->show();
            } else {
                this->plot->hide();
            }
            this->raise();
            this->setFocus();
            showPlot = !showPlot;
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

void Window::receiveNewMeassurementWindow(uint8_t* window, double *time, flip_type type)
{
    qDebug("Updateing curve");
    for(int j=0; j < LatencyModel::measurementWindowSize; j++){
        this->yData[j] = window[j];
    }
    yData[0] = 1.0;
    yData[1] = 10.0;
    yData[2] = 100.0;
    //QwtPointArrayData* d = new QwtPointArrayData( time, yData, LatencyModel::measurementWindowSize);
    QwtPointArrayData* d = new QwtPointArrayData( time, yData, 3);
    updateCurveIdx = (updateCurveIdx+1)%nCurves;
    this->curves[type][this->updateCurveIdx]->setData( d );
    this->cPlots[type]->replot();
}

void Window::receiveLatencyUpdate(LatencyModel* lm)
{
    /*
    memcpy( this->xData, lm->getSampleTimes(), sizeof(double)*LatencyModel::measurementWindowSize);

    return;
    //Only update curves if plot is visible
    if( this->showPlot )
        return;

    qDebug("Updating plots");

    memcpy( this->xData, lm->getSampleTimes(), sizeof(double)*LatencyModel::measurementWindowSize);


    LatencyModel::adcWindow* adc;
    //uint8_t adcData[2][measurementHistoryLength][measurementWindowSize];
    adc = lm->getAdcData();

    for(int i=0; i < this->nCurves; i++)
    {
        for(int j=0; j < LatencyModel::measurementWindowSize; j++){
            this->yData[i][j] = (*adc)[BLACK_TO_WHITE][i][j];
        }
        QwtPointArrayData* d = new QwtPointArrayData( xData, yData[i], LatencyModel::measurementWindowSize);
        this->curves[BLACK_TO_WHITE][i]->setData( d );
    }
    this->cPlots[BLACK_TO_WHITE]->replot();

    for(int i=0; i < this->nCurves; i++)
    {
        for(int j=0; j < LatencyModel::measurementWindowSize; j++){
            this->yData[i][j] = (*adc)[WHITE_TO_BLACK][i][j];
        }
        QwtPointArrayData* d = new QwtPointArrayData( xData, yData[i], LatencyModel::measurementWindowSize);
        this->curves[WHITE_TO_BLACK][i]->setData( d );
    }
    this->cPlots[WHITE_TO_BLACK]->replot();
    */
}

SubWindow::SubWindow(QWidget* parent) : QWidget( parent , Qt::Window ) {}
