#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "qdebug.h"
#include "qthread.h"

#include "workingthread.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->var = 0;

    //Start a thread and execute doProcessing()
    QThread* thread = new QThread();
    workingThread* worker = new workingThread( &(this->var) );

    worker->moveToThread(thread);
    //connect(worker, SIGNAL(error(QString)), this, SLOT(errorString(QString)));
    connect(thread, SIGNAL(started()), worker, SLOT(doProcessing()));
    connect(worker, SIGNAL(finished()), this, SLOT(onThreadQuit()));
    connect(worker, SIGNAL(finished()), worker, SLOT(deleteLater()));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    thread->start();


    for(double d = 0;d < 10e6; d++);
    qDebug("Main: set var = 1");
    this->var = 1;
    for(double d = 0;d < 10e6; d++);
    this->var = 2;
    qDebug("Main: set var = 2");
    for(double d = 0;d < 10e6; d++);
    this->var = 3;
    qDebug("Main: set var = 3");

    qDebug("Main finished ...");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onThreadQuit()
{
    qDebug("Main: Worker thread quited. var = %d", var);
}
