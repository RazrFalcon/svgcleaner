QT       += core gui svg
TEMPLATE  = app
DESTDIR   =../../bin
TARGET      = SVGCleaner
unix:TARGET = svgcleaner-gui
mac:TARGET  = SVGCleaner

SOURCES  += main.cpp \
            thumbwidget.cpp \
            mainwindow.cpp \
            cleanerthread.cpp \
            someutils.cpp \
            itemwidget.cpp \
            aboutdialog.cpp \
            lineedit.cpp \
            wizarddialog.cpp \
            iconswidget.cpp \
            filefinder.cpp
		
HEADERS  += thumbwidget.h \
            mainwindow.h \
            cleanerthread.h \
            arguments.h \
            someutils.h \
            itemwidget.h \
            aboutdialog.h \
            lineedit.h \
            wizarddialog.h \
            iconswidget.h \
            filefinder.h

FORMS    += thumbwidget.ui \
            mainwindow.ui \
            aboutdialog.ui \
            wizarddialog.ui

RESOURCES   += ../../icons/icons.qrc
CODECFORSRC  = UTF-8
windows:RC_FILE = ../../icons/icon.rc
mac:ICON        = ../../icons/svgcleaner.icns

include(../../translations/translations.pri)
