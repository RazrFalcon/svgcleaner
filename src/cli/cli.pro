QT      += core
QT      -= gui

TARGET   = svgcleaner-cli
CONFIG  += console
CONFIG  -= app_bundle
DESTDIR  = ../../bin
TEMPLATE = app
DEFINES *= QT_USE_QSTRINGBUILDER
DEFINES *= QT_NO_CAST_FROM_ASCII

QMAKE_CXXFLAGS += -Wextra
QMAKE_CXXFLAGS_RELEASE += -fno-rtti -fno-exceptions

SOURCES += \
    basecleaner.cpp \
    enums.cpp \
    keys.cpp \
    main.cpp \
    paths/paths.cpp \
    remover.cpp \
    replacer.cpp \
    svgdom/svgdom.cpp \
    tools.cpp \
    transform.cpp \
    paths/pathsegment.cpp \
    paths/pathbbox.cpp \
    svgdom/svgparser.cpp \
    stringwalker.cpp \
    mindef.cpp

HEADERS += \
    basecleaner.h \
    enums.h \
    keys.h \
    paths/paths.h \
    remover.h \
    replacer.h \
    svgdom/svgdom.h \
    tools.h \
    transform.h \
    paths/pathsegment.h \
    mindef.h \
    paths/pathbbox.h \
    svgdom/svgparser.h \
    stringwalker.h

include(../3rdparty/systemsemaphore/systemsemaphore.pri)
