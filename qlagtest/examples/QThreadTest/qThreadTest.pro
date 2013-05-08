#-------------------------------------------------
#
# Project created by QtCreator 2013-05-08T11:26:14
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qThreadTest
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    workingthread.cpp

HEADERS  += mainwindow.h \
    workingthread.h

FORMS    += mainwindow.ui
