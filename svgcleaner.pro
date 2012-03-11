QT       += core gui svg
TEMPLATE  = app
VERSION   = 0.4
unix:TARGET    = svgcleaner-gui
windows:TARGET = SVGCleaner

SOURCES  += src/main.cpp \
            src/thumbwidget.cpp \
            src/mainwindow.cpp \
            src/cleanerthread.cpp \
            src/someutils.cpp \
            src/itemwidget.cpp \
            src/aboutdialog.cpp \
            src/lineedit.cpp \
            src/wizarddialog.cpp \
            src/iconswidget.cpp
		
HEADERS  += src/thumbwidget.h \
            src/mainwindow.h \
            src/cleanerthread.h \
            src/arguments.h \
            src/someutils.h \
            src/itemwidget.h \
            src/aboutdialog.h \
            src/lineedit.h \
            src/wizarddialog.h \
            src/iconswidget.h

FORMS    += src/thumbwidget.ui \
            src/mainwindow.ui \
            src/aboutdialog.ui \
            src/wizarddialog.ui

RESOURCES   += icons/icons.qrc
CODECFORSRC  = UTF-8
windows:RC_FILE = icons/icon.rc

include(translations/translations.pri)

unix {
    isEmpty (PREFIX):PREFIX = /usr

    INSTALLS           += target desktop logo script presets interface translations

    desktop.path        = $$PREFIX/share/applications
    desktop.files      += svgcleaner.desktop
    logo.path           = $$PREFIX/share/icons/hicolor/scalable/apps
    logo.files         += icons/svgcleaner.svg
    interface.path      = $$PREFIX/share/svgcleaner
    interface.files    += interface.xml
    presets.path        = $$PREFIX/share/svgcleaner/presets
    presets.files      += presets/Soft.preset presets/Normal.preset presets/Optimal.preset
    translations.path   = $$PREFIX/share/svgcleaner/translations
    translations.files += svgcleaner_cs_CS.qm svgcleaner_ru_RU.qm svgcleaner_uk_UA.qm
    script.path         = $$PREFIX/bin
    script.files       += svgcleaner.pl
    target.path         = $$PREFIX/bin
}
