#-------------------------------------------------
#
# Project created by QtCreator 2013-09-30T12:57:00
#
#-------------------------------------------------

QT       += core gui serialport network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = zboss_sniffer
TEMPLATE = app


SOURCES += main.cpp\
        snifferwindow.cpp \
    pcapdumper.cpp \
    snifferserialdevice.cpp \
    wiresharkbridge.cpp \
    sniffercontroller.cpp

HEADERS  += snifferwindow.h \
    pcapdumper.h \
    snifferserialdevice.h \
    wiresharkbridge.h \
    sniffercontroller.h

FORMS    += snifferwindow.ui

RESOURCES += \
    icons/icons.qrc
