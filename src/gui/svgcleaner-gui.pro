QT       += core gui svg
TEMPLATE  = app
unix:TARGET    = svgcleaner-gui
windows:TARGET = SVGCleaner

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

include(../../translations/translations.pri)

unix { # // FIXME move to .pri
    isEmpty (PREFIX):PREFIX = /usr

    INSTALLS           += target desktop logo script presets translations

    desktop.path        = $$PREFIX/share/applications
    desktop.files      += svgcleaner.desktop

    logo.path           = $$PREFIX/share/icons/hicolor/scalable/apps
    logo.files         += icons/svgcleaner.svg

    presets.path        = $$PREFIX/share/svgcleaner/presets
    presets.files      += presets/Soft.preset \
                          presets/Normal.preset \
                          presets/Optimal.preset

    translations.path   = $$PREFIX/share/svgcleaner/translations
    translations.files += svgcleaner_cs.qm \
                          svgcleaner_ru.qm \
                          svgcleaner_uk.qm \
                          svgcleaner_de.qm

    script.path         = $$PREFIX/bin
    script.files       += ../../src/cli/svgcleaner-cli

    target.path         = $$PREFIX/bin
}
