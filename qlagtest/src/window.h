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

#ifndef WINDOW_H
#define WINDOW_H

#include <QWidget>
#include <QKeyEvent>
#include <QLabel>

#include "timemodel.h"
#include "ringbuffer.hpp"
#include <vector>

class QwtPlot;
class QwtPlotCurve;
class LatencyModel;
class QMenuBar;

class flashingBGQPaint : public QWidget
{
    Q_OBJECT
public:
    explicit flashingBGQPaint(int flipRate, TimeModel *clock, RingBuffer<screenFlip> *store);

public slots:
    void receiveStart();
    void receiveStop();

protected:
    void paintEvent(QPaintEvent *event);

    QRect r;
    bool drawWhiteBG;
    bool redraw;
    QTimer* timer;
    TimeModel *clock;
    RingBuffer<screenFlip> *store;

signals:

public slots:
    void flipColor();

};

class Window : public QWidget
{
    Q_OBJECT

public:
    enum drawingType{ QPAINT, OPENGL };
    //explicit Window(enum drawingType drawing, TimeModel *tm, RingBuffer<screenFlip> *screenFlips);
	explicit Window(TimeModel *tm, RingBuffer<screenFlip> *screenFlips);

signals:
    void startMeassurement();
    void stopMeassurement();
    void doReset();
    void flashAdruino();
    void generateReport();
    void showLogWindow();
    void selectPort();

public slots:
    void receiveUnstableLatency();
    void receiveStableLatency();
    void receiveInvalidLatency();
    void receiveLatencyUpdate(LatencyModel *lm);
    void receiveNewMeassurementWindow(uint8_t* window, double* avgWindow, double* time, flip_type type);
    void rcvTogglePlot();
    void quit();    
    void recvOpenHelpPage();
    void rcvShowAbout();

protected:   
    virtual void keyPressEvent(QKeyEvent *event);
    virtual void keyReleaseEvent(QKeyEvent *event);
    QLabel* msg;
    QLabel* latency;
    //QwtPlot* plot[2];

    int nCurves;
    int updateCurveIdx[2];
    QWidget* plot;
    std::vector<QwtPlotCurve*> curves[2];
    QwtPlotCurve* meanCurves[2];
    QwtPlotCurve* vLine[2];
    QwtPlot* cPlots[2];
    bool showPlot;

    double* xData;
    double* yData;

    QMenuBar* createMenu();
    void createPlots();


private:    
};

// Simple QWidget extention that fowards all keyboard events to its parent
class SubWindow : public QWidget{
    Q_OBJECT
public:
    explicit SubWindow(QWidget* parent = NULL);
protected:
    virtual void keyPressEvent(QKeyEvent *event) { this->parent()->event(event); }
};



//! [0]

#endif
