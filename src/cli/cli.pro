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
    basecleaner.cpp \
    keys.cpp \
    paths.cpp \
    remover.cpp \
    replacer.cpp \
    tinyxml2.cpp \
    tools.cpp

HEADERS += \
    basecleaner.h \
    keys.h \
    paths.h \
    remover.h \
    replacer.h \
    tinyxml2.h \
    tools.h
