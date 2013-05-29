#-------------------------------------------------
#
# Project created by QtCreator 2013-05-08T13:17:45
#
#-------------------------------------------------

QT       += core
QT += serialport

QT       -= gui

TARGET = qSerialComm
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    lagtestserialportcomm.cpp \
    mainapp.cpp

HEADERS += \
    lagtestserialportcomm.h \
    mainapp.h
