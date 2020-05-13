TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        main.c

LIBS+=-llibcomm

# Path to libcomm projet
LIB_SRC_PATH = ../libComm

INCLUDEPATH += $$LIB_SRC_PATH/
INCLUDEPATH += $$LIB_SRC_PATH/devices

TARGET = example

CONFIG(debug, debug|release) {
    //QMAKE_CFLAGS += -O0
    LIBS+=-L$$LIB_SRC_PATH/build-win/debug/
    message(Debug configuration.)
}

CONFIG(release, debug|release) {
    LIBS+=-L$$LIB_SRC_PATH/build-win/release
    message(Release configuration.)
}
