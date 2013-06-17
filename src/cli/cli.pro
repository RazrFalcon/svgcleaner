QT      += core xml

TARGET   = svgcleaner-cli
CONFIG  += console
CONFIG  -= app_bundle
DESTDIR  =../../bin
TEMPLATE = app

SOURCES += main.cpp \
           replacer.cpp \
           remover.cpp \
           paths.cpp \
           tools.cpp \
           keys.cpp

HEADERS += replacer.h \
           remover.h \
           paths.h \
           tools.h \
           keys.h
