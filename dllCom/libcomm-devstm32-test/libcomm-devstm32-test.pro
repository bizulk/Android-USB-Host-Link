TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    device_stm32.c \
    main.c

HEADERS += \
    device_stm32.h

TARGET = proto_devstm32

LIBS+=-llibcomm

# Path to libcomm projet
LIB_SRC_PATH = ../libcomm

INCLUDEPATH += $$LIB_SRC_PATH/
INCLUDEPATH += $$LIB_SRC_PATH/devices

CONFIG(debug, debug|release) {
    QMAKE_CFLAGS += -O0
    LIBS+=-L$$LIB_SRC_PATH/../build-win/debug/
    message(Debug configuration.)
}

CONFIG(release, debug|release) {
    LIBS+=-L$$LIB_SRC_PATH/../build-win/release
    message(Release configuration.)
}
