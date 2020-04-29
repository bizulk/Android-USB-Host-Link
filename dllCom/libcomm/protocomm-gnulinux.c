#include "protocomm-gnulinux.h"

// pour avoir accès aux fonctions exclusives à GNU/Linux
#define _GNU_SOURCE
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>


int proto_initData_GnuLinux(proto_Data_GnuLinux* iodata, char const* path, uint8_t timeout_ds) {
	assert(iodata != NULL);
	
	// Ouverture du terminal
	int fileDescriptor = open(path, O_RDWR);
	if (fileDescriptor < 0) return -1;
	
	// Configuration du terminal
    struct termios tty;
    memset(&tty, 0, sizeof tty);
    if (tcgetattr(fileDescriptor, &tty) != 0) {
		close(fileDescriptor);
        return -2;
    }
    tty.c_cflag = tty.c_cflag
        & ~PARENB // Pas de bit de parité
        & ~CSTOPB // Un seul bit de stop
        | CS8     // 8 bits par byte
        | CREAD   // on active READ
        | CLOCAL; // on ignore les ctrl lines
    tty.c_lflag = tty.c_lflag
        & ~ICANON // Pas le mode canonique
        & ~ECHO   // Pas d'echo
        & ~ECHONL // Pas d'echo sur nouvelle ligne
        & ~ISIG   // On n'interprète pas INTR QUIT SUSP
        & ~IXON & ~IXOFF & ~ IXANY // Pas de s/w flow ctrl
        & ~IGNBRK & ~BRKINT & ~PARMRK & ~ISTRIP
        & ~INLCR & ~IGNCR & ~ ICRNL;
    tty.c_oflag &= ~OPOST & ~ONLCR;
    
    tty.c_cc[VTIME] = timeout_ds;
    tty.c_cc[VMIN] = 0;

    if (tcsetattr(fileDescriptor, TCSANOW, &tty) != 0) {
		close(fileDescriptor);
        return -3;
    }
    
    // Tout s'est bien passé
    iodata->fileDescriptor = fileDescriptor;
    return 0;
}

void proto_finalizeData_GnuLinux(proto_Data_GnuLinux* iodata) {
	assert(iodata != NULL);
	close(iodata->fileDescriptor);
}

static void gnulinux_write(void* iodata, uint8_t const* buffer, uint8_t size) {
	proto_Data_GnuLinux* data = iodata;
	write(data->fileDescriptor, buffer, size);
}

static uint8_t gnulinux_read(void* iodata, uint8_t* buffer, uint8_t bufferSize) {
	proto_Data_GnuLinux* data = iodata;
	return read(data->fileDescriptor, buffer, bufferSize);
}

proto_Device proto_getDevice_GnuLinux() {
	static proto_IfaceIODevice gnulinux_device = {
		.write = gnulinux_write, .read = gnulinux_read
	};
	return &gnulinux_device;
}
