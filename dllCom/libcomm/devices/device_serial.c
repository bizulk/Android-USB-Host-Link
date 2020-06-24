/// Ce fichier implémente un proto device de type port série sous windows
/// TODO serait contaténable avec Linux ? je sais plus...
/// L'implémentation permettrait l'ouverture du plusieurs port en parallèle
///
///

#include <stdlib.h>
#include <assert.h>
#include "proto_iodevice.h"
#include "device_serial.h"
#include <string.h>
#include <ctype.h>
#include <errno.h>


/******************************************************************************
 * TYPES & VARIABLES
******************************************************************************/

/* Complément de données nécessaire à la structure */
typedef struct
{
    int fileDescriptor; // Descripteur de fichier ouvert
} proto_dev_serial_t ;

/******************************************************************************
 * LOCAL PROTO
******************************************************************************/

///
/// \brief devserial_init : même chose que create, mais avec une instance alloué sur la pile
/// \warning Ne pas appeler "destroy" pour ce cas
/// \param this Instance pre-alloué
///
static void devserial_init(proto_Device_t this);

/******************************************************************************
 * FUNCTION
******************************************************************************/

void devserial_destroy(proto_Device_t this)
{
    assert(this && this->user);

    /// Fermeture des ports (todo).
    ///
    /// Liberation des structures
    this->close(this);
    free(this->user);
    free(this);
}

#ifdef __linux__
#warning "info - compiling for linux env"
// headers propres à Linux
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

/// Configure le FileDescriptor
/// Notamment vtime et vmin, qui définissent
/// le comportement bloquant (ou non) de la fonction read
/// retourne 0 si OK, -1 si erreur
static int setFdMode(int fd, int vtime, int vmin) {
    struct termios tty;
    memset(&tty, 0, sizeof tty);
    if (tcgetattr(fd, &tty) != 0) {
        return -1;
    }
    tty.c_cflag = ( tty.c_cflag
                        & ~PARENB // Pas de bit de parité
                        & ~CSTOPB ) // Un seul bit de stop
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
        return -1;
    }
    return 0;
}

static int devserial_close(struct proto_IfaceIODevice* this)
{
    assert(this);
	int ret = 0;

    proto_dev_serial_t* infos = this->user;

	if (infos->fileDescriptor >=0)
	{
		ret = close(infos->fileDescriptor);
		// Invalider le descripteur de fichier pour éviter toute utilisation ultérieure
		// Même si cela n'a pas marché on peut pas faire grand chose de plus ici
		infos->fileDescriptor = -1;
	}

	return ret;
}

static int devserial_open(struct proto_IfaceIODevice* this, const char * szPath)
{

    assert(this && szPath);

    proto_dev_serial_t* infos = this->user;

    devserial_close(this);

    int fd = open(szPath, O_RDWR);
    if (fd < 0)
        return fd;

    infos->fileDescriptor = fd;
    // vtime = 0, vmin = 0 : les appels à "read" sont non-bloquants
	// TODO SLI : c'était possible de les rendre bloquants, la fonction de haut niveau contrôlant le temps écoulé
    return setFdMode(infos->fileDescriptor, 0, 0);
}


static int devserial_read(struct proto_IfaceIODevice* this, void* buffer, uint8_t bufferSize, int16_t tout_ms)
{
    assert(this && buffer);
    UNUSED(tout_ms);
	proto_dev_serial_t* infos = this->user;
	if (infos->fileDescriptor < 0)
		return -1;
    return read(infos->fileDescriptor, buffer, bufferSize);
}

static int devserial_write(struct proto_IfaceIODevice* this, const void * buffer, uint8_t size)
{
    assert(this && buffer);
	proto_dev_serial_t* infos = this->user;
	if (infos->fileDescriptor < 0)
		return -1;
    while (size > 0) {
        int ret = write(infos->fileDescriptor, buffer, size);
        if (ret < 0)
            return -1;
        else {
            buffer += ret; // octets qui restent à transmettre
            size -= ret;
        }
    }
    return 0;
}

int devserial_setFD(proto_Device_t this, int fileDescriptor) {
    assert(this);

	if (fileDescriptor < 0)
		return -1;
    int ret = setFdMode(fileDescriptor, 0, 0);
    if (ret < 0)
        return -1;

    devserial_close(this);
    proto_dev_serial_t* infos = this->user;
    infos->fileDescriptor = fileDescriptor;
    return 0;
}

int devserial_getFD(proto_Device_t this) {

    return ((proto_dev_serial_t*) this->user)->fileDescriptor;

}

#else // if __linux__ not defined

static int devserial_open(struct proto_IfaceIODevice* this, const char * szPath)
{
    UNUSED(this);
    UNUSED(szPath);
    return -1;
}

static int devserial_close(struct proto_IfaceIODevice* this)
{
    UNUSED(this);
    return -1;
}

static int devserial_read(struct proto_IfaceIODevice* this, void* buffer, uint8_t bufferSize, int16_t tout_ms)
{
    UNUSED(this);
    UNUSED(buffer);
    UNUSED(bufferSize);
    UNUSED(tout_ms);
    return 0;
}

static int devserial_write(struct proto_IfaceIODevice* this, const void * buffer, uint8_t size)
{
    UNUSED(this);
    UNUSED(buffer);
    UNUSED(size);
    return 0;
}

int devserial_openFD(proto_Device_t _this, int fileDescriptor) {
    UNUSED(_this);
    UNUSED(fileDescriptor);
    return -1;
}


#endif

proto_Device_t devserial_create(void)
{
    /// allocation des structures et initialisation du pointeur
    proto_Device_t this  = (proto_Device_t)malloc( sizeof(proto_IfaceIODevice_t) );
    this->user = (proto_dev_serial_t*)malloc(sizeof(proto_dev_serial_t));
    devserial_init(this);

    return this;
}


void devserial_init(proto_Device_t this)
{
    assert(this);
    this->open = devserial_open;
    this->close = devserial_close;
    this->destroy = devserial_destroy;
    this->read = devserial_read;
    this->write = devserial_write;

    proto_dev_serial_t* infos = this->user;
    infos->fileDescriptor = -1; // Initialisation à la valeur "invalide" retournée par l'open
}
