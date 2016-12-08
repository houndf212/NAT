QT += core network gui widgets

TARGET = natclient
TEMPLATE = app

PRECOMPILED_HEADER = pch.h

CONFIG+=precompile_header
PRECOMPILED_HEADER = pch.h

include(../common/common.pri)

SOURCES += main.cpp \
    dialog.cpp

HEADERS += \
    dialog.h \
    pch.h

FORMS += \
    dialog.ui

