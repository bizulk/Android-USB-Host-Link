CONFIG -= qt

TEMPLATE = lib
DEFINES += LIBCOMM_LIBRARY

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    devices/device_emulslave.c \
    devices/device_serial.c \
    log.c \
    protocomm_ll.c \
    protocomm_master.c \
    protocomm_slave.c

HEADERS += \
    devices/device_emulslave.h \
    devices/device_serial.h \
    libcomm_global.h \
    log.h \
    proto_iodevice.h \
    protocomm_ll.h \
    protocomm_master.h \
    protocomm_slave.h

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target
