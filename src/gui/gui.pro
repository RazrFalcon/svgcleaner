QT       += core gui svg
TEMPLATE  = app
DESTDIR   = ../../bin
TARGET      = SVGCleaner
unix:!mac:TARGET = svgcleaner-gui

DEFINES *= QT_USE_QSTRINGBUILDER

SOURCES += \
    main.cpp \
    aboutdialog.cpp \
    cleaner.cpp \
    iconswidget.cpp \
    lineedit.cpp \
    mainwindow.cpp \
    settings.cpp \
    someutils.cpp \
    thumbwidget.cpp \
    wizarddialog.cpp \
    ../cli/keys.cpp \
    spinbox.cpp

HEADERS += \
    aboutdialog.h \
    arguments.h \
    cleaner.h \
    iconswidget.h \
    lineedit.h \
    mainwindow.h \
    settings.h \
    someutils.h \
    thumbwidget.h \
    wizarddialog.h \
    ../cli/keys.h \
    spinbox.h

FORMS += \
    aboutdialog.ui \
    mainwindow.ui \
    thumbwidget.ui \
    wizarddialog.ui

RESOURCES   += ../../icons/icons.qrc
CODECFORSRC  = UTF-8
win32:RC_FILE = ../../icons/icon.rc
mac:ICON        = ../../icons/svgcleaner.icns

include(../../translations/translations.pri)
