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
    svgelement.cpp \
    tinyxml2.cpp \
    tools.cpp \
    transform.cpp

HEADERS += \
    basecleaner.h \
    keys.h \
    paths.h \
    remover.h \
    replacer.h \
    svgelement.h \
    tinyxml2.h \
    tools.h \
    transform.h
