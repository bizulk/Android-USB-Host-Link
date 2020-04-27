#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <assert.h>
#include <string.h>

/// Ouvre un pseudo-terminal maître et son esclave.
/// Retourne -1 en cas d'erreur, et errno est mis à jour.
/// out_master et out_slave ne sont pas modifiés en cas d'erreur.
int open_both(int* out_master, int* out_slave) {
    assert(out_master != NULL);
    assert(out_slave != NULL);
    int master = open("/dev/ptmx", O_RDWR);
    if (master < 0
            || grantpt(master) < 0
            || unlockpt(master) < 0) {
        return -1;
    }
    int slave = open(ptsname(master), O_RDWR);
    if (slave < 0) {
        close(master);
        return -1;
    }
    *out_master = master;
    *out_slave = slave;
    return 0;
}

/// Configure le FileDescriptor
/// Notamment vtime et vmin, qui définissent
/// le comportement bloquant (ou non) de la fonction read
int setFdMode(int fd, int vtime, int vmin) {
    struct termios tty;
    memset(&tty, 0, sizeof tty);
    if (tcgetattr(fd, &tty) != 0) {
        puts("setFdMode error");
        return -1;
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
    
    tty.c_cc[VTIME] = vtime;
    tty.c_cc[VMIN] = vmin;

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        puts("setFdMode error");
        return -1;
    }
    return 0;
}

char buf[50];
char const msg[] = "Hello, World!";
int main() {
    int master, slave;
    if (open_both(&master, &slave) < 0) {
        perror("open_both() a échoué");
        return EXIT_FAILURE;
    }
    puts("Fd ouverts");
    setFdMode(master, 10, 255);
    setFdMode(slave, 10, 255);

    write(master, msg, sizeof(msg));
    read(slave, buf, sizeof(buf));
    printf("Read: '%s'\n", buf);


    close(slave);
    close(master);
    return EXIT_SUCCESS;
}
