QT += core
QT -= gui
QT += network

CONFIG += c++11

TARGET = Server
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    server.cpp \
    worker.cpp \
    tasks.cpp \
    logger.cpp

HEADERS += \
    server.h \
    worker.h \
    hcommon.h \
    tasks.h \
    logger.h
