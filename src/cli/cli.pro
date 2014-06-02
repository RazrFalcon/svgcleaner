QT      += core
QT      -= gui

TARGET   = svgcleaner-cli
CONFIG  += console
CONFIG  -= app_bundle
DESTDIR  = ../../bin
TEMPLATE = app
DEFINES *= QT_USE_QSTRINGBUILDER

SOURCES += \
    basecleaner.cpp \
    enums.cpp \
    keys.cpp \
    main.cpp \
    paths.cpp \
    remover.cpp \
    replacer.cpp \
    svgdom.cpp \
    tools.cpp \
    transform.cpp

HEADERS += \
    basecleaner.h \
    enums.h \
    keys.h \
    paths.h \
    remover.h \
    replacer.h \
    svgdom.h \
    tools.h \
    transform.h
