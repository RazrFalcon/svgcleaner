QT       += core gui xml svg
TEMPLATE  = app
VERSION   = 0.3
unix:TARGET    = svgcleaner-gui
windows:TARGET = SVGCleaner

SOURCES  += src/main.cpp \
            src/guiinfo.cpp \
            src/thumbwidget.cpp \
            src/mainwindow.cpp \
            src/cleanerthread.cpp \
            src/someutils.cpp \
            src/itemwidget.cpp \
            src/aboutdialog.cpp \
            src/lineedit.cpp \
            src/wizarddialog.cpp
		
HEADERS  += src/guiinfo.h \
            src/thumbwidget.h \
            src/mainwindow.h \
            src/cleanerthread.h \
            src/arguments.h \
            src/someutils.h \
            src/itemwidget.h \
            src/aboutdialog.h \
            src/lineedit.h \
            src/wizarddialog.h

FORMS    += src/thumbwidget.ui \
            src/mainwindow.ui \
            src/aboutdialog.ui \
            src/wizarddialog.ui

OTHER_FILES += interface.xml
RESOURCES   += icons/icons.qrc
CODECFORSRC  = UTF-8
windows:RC_FILE = icons/icon.rc

#include(translations/translations.pri)

unix {
    isEmpty (PREFIX):PREFIX = /usr

    INSTALLS          += target desktop logo script presets interface trans

    desktop.path       = $$PREFIX/share/applications
    desktop.files     += svgcleaner.desktop
    logo.path          = $$PREFIX/share/icons/hicolor/scalable/apps
    logo.files        += icons/svgcleaner.svg
#    trans.path         = $$PREFIX/share/svgcleaner/translations
#    trans.files       += svgcleaner_ru.qm svgcleaner_cs.qm
    interface.path     = $$PREFIX/share/svgcleaner
    interface.files   += interface.xml
    presets.path       = $$PREFIX/share/svgcleaner/presets
    presets.files     += presets/soft.preset presets/normal.preset \
                         presets/optimal.preset presets/vacuum-defs.preset
    script.path        = $$PREFIX/bin
    script.files      += svgcleaner
    target.path        = $$PREFIX/bin
}
