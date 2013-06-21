TRANSLATIONS += ../../translations/svgcleaner_cs.ts \
                ../../translations/svgcleaner_ru.ts \
                ../../translations/svgcleaner_uk.ts \
                ../../translations/svgcleaner_de.ts

CODECFORTR = UTF-8

isEmpty(QMAKE_LRELEASE) {
    !exists($$QMAKE_LRELEASE) { QMAKE_LRELEASE = lrelease }
}

# fix for ArchLinux
!exists(QMAKE_LRELEASE) {
    QMAKE_LRELEASE = lrelease-qt
}

updateqm.input = TRANSLATIONS
updateqm.output = ${QMAKE_FILE_BASE}.qm
updateqm.commands = $$QMAKE_LRELEASE -silent ${QMAKE_FILE_IN} -qm ${QMAKE_FILE_BASE}.qm
updateqm.CONFIG += no_link target_predeps
QMAKE_EXTRA_COMPILERS += updateqm
