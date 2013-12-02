QT      += core
QT      -= gui

TARGET   = svgcleaner-cli
CONFIG  += console
CONFIG  -= app_bundle
DESTDIR  = ../../bin
TEMPLATE = app
DEFINES *= QT_USE_QSTRINGBUILDER

SOURCES += \
    main.cpp \
    replacer.cpp \
    remover.cpp \
    paths.cpp \
    tools.cpp \
    keys.cpp \
    tinyxml2.cpp

HEADERS += \
    replacer.h \
    remover.h \
    paths.h \
    tools.h \
    keys.h \
    tinyxml2.h
