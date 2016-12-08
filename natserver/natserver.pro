QT += core network
QT -= gui

TARGET = natserver
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

CONFIG+=precompile_header
PRECOMPILED_HEADER = pch.h

include(../common/common.pri)

SOURCES += main.cpp \
    server.cpp \
    udpserver.cpp

HEADERS += \
    server.h \
    udpserver.h \
    pch.h
