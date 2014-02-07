TEMPLATE = subdirs
SUBDIRS = src/cli src/gui
CONFIG += ordered

unix:!mac {
    isEmpty (PREFIX):PREFIX = /usr

    INSTALLS           += desktop logo translations bin

    desktop.path        = $$PREFIX/share/applications
    desktop.files      += svgcleaner.desktop

    logo.path           = $$PREFIX/share/icons/hicolor/scalable/apps
    logo.files         += icons/svgcleaner.svg

    translations.path   = $$PREFIX/share/svgcleaner/translations
    translations.files += bin/svgcleaner_cs.qm \
                          bin/svgcleaner_ru.qm \
                          bin/svgcleaner_uk.qm \
                          bin/svgcleaner_it.qm \
                          bin/svgcleaner_fr.qm \
                          bin/svgcleaner_de.qm

    bin.path            = $$PREFIX/bin
    bin.files          += bin/svgcleaner-cli bin/svgcleaner-gui
}
