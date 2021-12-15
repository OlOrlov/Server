QT += core
QT -= gui
QT += network
QT += widgets

CONFIG += c++11

TARGET = Server
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    server.cpp \
    tasks.cpp \
    logger.cpp

HEADERS += \
    server.h \
    hcommon.h \
    tasks.h \
    logger.h
