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

#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_legend.h>
#include <QMenuBar>

#include <QCoreApplication>
#include "latencymodel.h"

#include <QDesktopServices>
#include <QMessageBox>
#include <QPainter>

//Window::Window(enum drawingType drawing, TimeModel *tm, RingBuffer<screenFlip> *screenFlips)
Window::Window(TimeModel *tm, RingBuffer<screenFlip> *screenFlips) :
    showPlot(false)
{    
    qDebug("Creating main window ...");
    QWidget* flipWindow;
	enum drawingType drawing = Window::QPAINT;
    setWindowTitle("Lagtest - How fast is your display");
    this->resize(640, 480);

    QVBoxLayout *layout = new QVBoxLayout;
    this->msg = new QLabel("Put the light sensor ontop of the white area. When ready press the space bar.");
    this->latency = new QLabel("");

    msg->setAlignment(Qt::AlignHCenter);
    latency->setAlignment(Qt::AlignHCenter);

    QHBoxLayout* buttonLayout = new QHBoxLayout;

    layout->addWidget(msg, 0);
    layout->addWidget(latency, 0);

    flipWindow = new flashingBGQPaint(500, tm, screenFlips);

    connect( this, SIGNAL(startMeassurement()), flipWindow, SLOT(receiveStart()) );
    connect( this, SIGNAL(stopMeassurement()), flipWindow, SLOT(receiveStop()) );

    flipWindow->resize(150, 140);
    flipWindow->show();
    layout->addWidget(flipWindow, 2);

    layout->setMenuBar( this->createMenu() );
    this->createPlots();

    this->setLayout(layout);

    qDebug("Main Window done!");    
}


QMenuBar* Window::createMenu()
{
    //Create Menu
    QMenuBar* menuBar = new QMenuBar;

    QMenu* fileMenu = new QMenu(tr("&File"), this);
    QAction* writeProtocolAction = fileMenu->addAction(tr("&Create Report"));
    QAction* exitAction = fileMenu->addAction(tr("E&xit"));
    menuBar->addMenu(fileMenu);

    QMenu* optionsMenu = new QMenu(tr("&Options"), this);
    QAction* selectPortAction = optionsMenu->addAction(tr("Select &Port"));
    QAction* flashAction = optionsMenu->addAction(tr("Fl&ash Adruino"));
    QAction* plotAction = optionsMenu->addAction(tr("Show &graph"));
    QAction* showLogAction = optionsMenu->addAction(tr("Show &Logs"));
    menuBar->addMenu(optionsMenu);

    QMenu* helpMenu = new QMenu(tr("&Help"), this);
    QAction* helpPageAction = helpMenu->addAction(tr("Goto &Introduction Page"));
    QAction* aboutAction = helpMenu->addAction(tr("&About"));
    menuBar->addMenu(helpMenu);

    connect(exitAction, SIGNAL(triggered()), this, SLOT(quit()));
    connect(flashAction, SIGNAL(triggered()), this, SIGNAL(flashAdruino()));
    connect(writeProtocolAction, SIGNAL(triggered()), this, SIGNAL(generateReport()));
    connect(helpPageAction, SIGNAL(triggered()), this, SLOT(recvOpenHelpPage()));
    connect(plotAction, SIGNAL(triggered()), this, SLOT(rcvTogglePlot()));
    connect(aboutAction, SIGNAL(triggered()), this, SLOT( rcvShowAbout()) );
    connect(showLogAction, SIGNAL(triggered()), this, SIGNAL(showLogWindow()));
    connect(selectPortAction, SIGNAL(triggered()), this, SIGNAL(selectPort()));


    return menuBar;
}


void Window::createPlots()
{

    qDebug("Preparing plotting");
    this->xData = (double*) malloc(sizeof(double) * LatencyModel::measurementWindowSize );
    this->yData = (double*) malloc(sizeof(double) * LatencyModel::measurementWindowSize);

    this->plot = new SubWindow(this);
    QVBoxLayout *plotLayout = new QVBoxLayout;

    this->cPlots[BLACK_TO_WHITE] = new QwtPlot( QwtText("Black to White") );
    this->cPlots[WHITE_TO_BLACK] = new QwtPlot( QwtText("White to Black") );

    cPlots[BLACK_TO_WHITE]->setCanvasBackground(QBrush(Qt::lightGray));
    cPlots[WHITE_TO_BLACK]->setCanvasBackground(QBrush(Qt::lightGray));

    //cPlots[BLACK_TO_WHITE]->setAxisScale(QwtPlot::yLeft, -10, 50);
    //cPlots[WHITE_TO_BLACK]->setAxisScale(QwtPlot::yLeft, -10, 50);
    //cPlots[BLACK_TO_WHITE]->setFooter( "Latency [ms]" );
    cPlots[BLACK_TO_WHITE]->setAxisTitle( QwtPlot::xBottom, "Latency [ms]" );
    cPlots[WHITE_TO_BLACK]->setAxisTitle( QwtPlot::xBottom, "Latency [ms]" );
    //cPlots[WHITE_TO_BLACK]->setFooter( "Latency [ms]" );    

    //QwtLegend* l = new QwtLegend();
    //QwtLegend* l2 = new QwtLegend();
    //l->setWindowTitle( "ADC" );
    //l2->setWindowTitle( "ADC" );
    //cPlots[BLACK_TO_WHITE]->insertLegend( l );
    //cPlots[WHITE_TO_BLACK]->insertLegend( l2 );


    updateCurveIdx[BLACK_TO_WHITE] = 0;
    updateCurveIdx[WHITE_TO_BLACK] = 0;

    this->nCurves = LatencyModel::measurementHistoryLength;
    int colors[] = { Qt::yellow, Qt::darkYellow, Qt::gray, Qt::darkGray, Qt::black, Qt::green, Qt::darkGreen, Qt::cyan, Qt::darkCyan, Qt::magenta, Qt::darkMagenta };
    int nColors = sizeof(colors)/sizeof(int);
    std::vector<QColor> colorObj;

    for( int i=0; i < nColors; i++){
        QColor c((Qt::GlobalColor) colors[i%nColors]);
        c.setAlpha( 100 );
        colorObj.push_back( c );
    }
    qDebug("nColors %d", nColors);

    QPen p;

    p.setStyle(Qt::DashLine);
    p.setWidth( 3 );
    p.setColor( Qt::blue );
    this->vLine[WHITE_TO_BLACK] = new QwtPlotCurve();
    this->vLine[BLACK_TO_WHITE] = new QwtPlotCurve();
    this->vLine[WHITE_TO_BLACK]->setPen( p );
    this->vLine[BLACK_TO_WHITE]->setPen( p );
    this->vLine[WHITE_TO_BLACK]->hide();
    this->vLine[BLACK_TO_WHITE]->hide();
    this->vLine[WHITE_TO_BLACK]->attach( cPlots[WHITE_TO_BLACK] );
    this->vLine[BLACK_TO_WHITE]->attach( cPlots[BLACK_TO_WHITE] );

    p.setWidth( 3 );
    p.setStyle(Qt::SolidLine);
    p.setColor( Qt::red );
    this->meanCurves[WHITE_TO_BLACK] = new QwtPlotCurve();
    this->meanCurves[BLACK_TO_WHITE] = new QwtPlotCurve();
    this->meanCurves[WHITE_TO_BLACK]->setPen( p );
    this->meanCurves[BLACK_TO_WHITE]->setPen( p );
    this->meanCurves[WHITE_TO_BLACK]->attach( cPlots[WHITE_TO_BLACK] );
    this->meanCurves[BLACK_TO_WHITE]->attach( cPlots[BLACK_TO_WHITE] );

    p.setStyle(Qt::SolidLine);
    p.setWidth( 1 );

    for(int i=0; i < nCurves; i++)
    {
        p.setColor( (QColor) colorObj[i%nColors] );

        curves[BLACK_TO_WHITE].push_back( new QwtPlotCurve() );
        curves[WHITE_TO_BLACK].push_back( new QwtPlotCurve() );

        curves[BLACK_TO_WHITE][i]->setPen( p );
        curves[WHITE_TO_BLACK][i]->setPen( p );

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

    plot->resize( 700 , 500);
    qDebug("Preparing plots done ...");
}


void Window::keyReleaseEvent(QKeyEvent *event)
{
    switch (event->key())
    {
        case Qt::Key_Space:
        {
            if( event->isAutoRepeat() )
                break;

            this->msg->setText( "Remain still. Calculating Latency ..." );
            emit startMeassurement();
            emit doReset();
            break;
        }
    }
}

void Window::keyPressEvent(QKeyEvent *event)
{
    //qDebug("Pressed key %c", event->key() );

    switch (event->key()) {       
    case Qt::Key_D:{
            this->rcvTogglePlot();
            break;
        }
    case Qt::Key_Q:{
            this->quit();
        }
    }        
}

void Window::rcvShowAbout()
{
    QMessageBox::about(this, tr("Lagtest - How slow is your display?"), tr("Some nice text here ...") );
}

void Window::rcvTogglePlot()
{
    if( this->showPlot ) {
        this->plot->show();
    } else {
        this->plot->hide();
    }
    this->raise();
    this->setFocus();
    showPlot = !showPlot;
}

void Window::receiveInvalidLatency()
{
    this->msg->setText( "Invalid Latency. Adjust the position of the light sensor to be ontop of the blinking area." );
    this->latency->clear();

    this->vLine[WHITE_TO_BLACK]->hide();
    this->vLine[BLACK_TO_WHITE]->hide();
}

void Window::receiveUnstableLatency()
{
    this->msg->setText( "Unstable Latency. Remain still. Calculating Latency ..." );
    this->latency->clear();

    this->vLine[WHITE_TO_BLACK]->hide();
    this->vLine[BLACK_TO_WHITE]->hide();
}

void Window::receiveStableLatency()
{    
    this->msg->setText( "Found a Latency of" );    
}

void Window::receiveNewMeassurementWindow(uint8_t* window, double *avgWindow, double *time, flip_type type)
{    
    //qDebug("Updateing curve %d",updateCurveIdx[type]);
    for(int j=0; j < LatencyModel::measurementWindowSize; j++)
    {
        this->yData[j] = window[j];
        this->xData[j] = time[j] / 1000000.0;
    }
    updateCurveIdx[type] = (updateCurveIdx[type]+1)%nCurves;
    this->curves[type][this->updateCurveIdx[type]]->setSamples( xData, yData, LatencyModel::measurementWindowSize );

    //Update Mean curve
    this->meanCurves[type]->setSamples( xData, avgWindow, LatencyModel::measurementWindowSize );

    this->cPlots[type]->replot();
}

void Window::receiveLatencyUpdate(LatencyModel* lm)
{
    QString str;
    double ll,al;
    ll = lm->getLastLatency();
    al = lm->getAvgLatency();

    this->msg->setText( "Found a Latency of" );
    this->latency->setText( str.sprintf("Last Latency %.2f ms , Avg. Latency %.2f|%.2f ms", ll/1000000.0, al/1000000.0, lm->getAvgLatencySD()/1000000.0) );

    double x[2], y[2];
    x[0] = al / 1000000.0; y[0] = -10.0;
    x[1] = al / 1000000.0; y[1] = this->meanCurves[WHITE_TO_BLACK]->maxYValue() + 10;
    this->vLine[WHITE_TO_BLACK]->setSamples( x, y, 2 );
    this->vLine[BLACK_TO_WHITE]->setSamples( x, y, 2 );

    this->vLine[WHITE_TO_BLACK]->show();
    this->vLine[BLACK_TO_WHITE]->show();

}


void Window::recvOpenHelpPage(){
    QDesktopServices::openUrl(QUrl("http://lagtest.org/app-help", QUrl::TolerantMode));
}

void Window::quit(){
    emit QCoreApplication::quit();
}

SubWindow::SubWindow(QWidget* parent) : QWidget( parent , Qt::Window ) {}

// ########################################################################################################
// ####             Flashing Background Using QPaint                                                    ###
// ########################################################################################################


flashingBGQPaint::flashingBGQPaint(int flipRate, TimeModel* clock, RingBuffer<screenFlip> *store) :
    QWidget(0),
    r(this->rect()),
    drawWhiteBG(false),
    clock(clock),
    store(store)
{
    this->timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(flipColor()));
    timer->setInterval(flipRate);
}


void flashingBGQPaint::flipColor()
{    
    emit update(); //Will produce a paint event, and make the screen update
    this->drawWhiteBG = !this->drawWhiteBG;
}

void flashingBGQPaint::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.fillRect(r, (this->drawWhiteBG) ? Qt::white : Qt::black );

    screenFlip sf;
    sf.type = this->drawWhiteBG ? BLACK_TO_WHITE : WHITE_TO_BLACK;
    sf.local = this->clock->getCurrentTime();

    this->store->put( &sf );
}

void flashingBGQPaint::receiveStart(){
    this->timer->start();
}

void flashingBGQPaint::receiveStop(){
    this->timer->stop();
    this->drawWhiteBG = false;
    emit update();
}
