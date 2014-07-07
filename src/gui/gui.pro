QT       += core gui svg
TEMPLATE  = app
DESTDIR   = ../../bin
TARGET    = SVGCleaner
unix:!mac:TARGET = svgcleaner-gui

DEFINES *= QT_USE_QSTRINGBUILDER

SOURCES += \
    aboutdialog.cpp \
    cleanerthread.cpp \
    filesview.cpp \
    iconswidget.cpp \
    main.cpp \
    mainwindow.cpp \
    settings.cpp \
    someutils.cpp \
    spinbox.cpp \
    thumbwidget.cpp \
    wizarddialog.cpp \
    ../cli/keys.cpp

HEADERS += \
    aboutdialog.h \
    arguments.h \
    cleanerthread.h \
    filesview.h \
    iconswidget.h \
    mainwindow.h \
    settings.h \
    someutils.h \
    spinbox.h \
    thumbwidget.h \
    wizarddialog.h \
    ../cli/keys.h

FORMS += \
    aboutdialog.ui \
    mainwindow.ui \
    thumbwidget.ui \
    wizarddialog.ui

RESOURCES    += ../../icons/icons.qrc
CODECFORSRC   = UTF-8
win32:RC_FILE = ../../icons/icon.rc
mac:ICON      = ../../icons/svgcleaner.icns

include(../3rdparty/systemsemaphore/systemsemaphore.pri)

include(../../translations/translations.pri)
