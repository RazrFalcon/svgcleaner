TEMPLATE = subdirs
SUBDIRS = src/cli src/gui
CONFIG += ordered

unix {
    isEmpty (PREFIX):PREFIX = /usr

    INSTALLS           += bin desktop logo presets translations

    desktop.path        = $$PREFIX/share/applications
    desktop.files      += svgcleaner.desktop

    logo.path           = $$PREFIX/share/icons/hicolor/scalable/apps
    logo.files         += icons/svgcleaner.svg

    presets.path        = $$PREFIX/share/svgcleaner/presets
    presets.files      += presets/Soft.preset \
                          presets/Normal.preset \
                          presets/Optimal.preset

    translations.path   = $$PREFIX/share/svgcleaner/translations
    translations.files += bin/svgcleaner_cs.qm \
                          bin/svgcleaner_ru.qm \
                          bin/svgcleaner_uk.qm \
                          bin/svgcleaner_de.qm

    bin.path            = $$PREFIX/bin
    bin.files          += bin/svgcleaner-cli bin/svgcleaner-gui
}
