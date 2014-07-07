# use only on Windows and Linux
win32|unix:!macx {

    DEFINES *= USE_IPC

    SOURCES += $$PWD/systemsemaphore.cpp

    win32: SOURCES += $$PWD/systemsemaphore_win.cpp
    unix:  SOURCES += $$PWD/systemsemaphore_unix.cpp

    HEADERS += \
        $$PWD/systemsemaphore.h \
        $$PWD/systemsemaphore_p.h
}
