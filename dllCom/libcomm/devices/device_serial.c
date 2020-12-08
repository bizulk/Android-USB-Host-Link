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
#include "log.h"

/******************************************************************************
 * TYPES & VARIABLES
******************************************************************************/

/* Complément de données nécessaire à la structure */
typedef struct
{
    int fd; // Descripteur de fichier ouvert
} proto_dev_serial_t ;

/******************************************************************************
 * LOCAL PROTO
******************************************************************************/

///
/// \brief devserial_init : même chose que create, mais avec une instance alloué sur la pile
/// \warning Ne pas appeler "destroy" pour ce cas
/// \param _this Instance pre-alloué
///
static void devserial_init(proto_Device_t _this);

/******************************************************************************
 * FUNCTION
******************************************************************************/

void devserial_destroy(proto_Device_t _this)
{
    assert(_this && _this->user);

    /// Fermeture des ports (todo).
    ///
    /// Liberation des structures
    _this->close(_this);
    free(_this->user);
    free(_this);
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

static int devserial_close(struct proto_IfaceIODevice* _this)
{
    assert(_this);
	int ret = 0;

    proto_dev_serial_t* infos = _this->user;

	if (infos->fd >=0)
	{
		ret = close(infos->fd);
		// Invalidate the descriptor to block further use.
		infos->fd = -1;
	}

	return ret;
}

static int devserial_open(struct proto_IfaceIODevice* _this, const char * szPath)
{
    assert(_this && szPath);

    proto_dev_serial_t* infos = _this->user;

    devserial_close(_this);

	/* allow empty path : that means that the fd will be provided */
	if (strlen(szPath) > 0)
	{
		int fd = open(szPath, O_RDWR);
		LOG("OPEN fd :%d", fd);
		if (fd < 0)
			return fd;
		infos->fd = fd;
		// vtime = 0, vmin = 0 : les appels à "read" sont non-bloquants
		// TODO SLI : c'était possible de les rendre bloquants, la fonction de haut niveau contrôlant le temps écoulé
		return setFdMode(infos->fd, 0, 0);
	}
	
	return 0;
}


static int devserial_read(struct proto_IfaceIODevice* _this, void* buffer, uint8_t bufferSize, int16_t tout_ms)
{
	int ret = 0;
    assert(_this && buffer);
    UNUSED(tout_ms);
	proto_dev_serial_t* infos = _this->user;
	if (infos->fd < 0)
		return -1;
	ret = read(infos->fd, buffer, bufferSize);
	if (ret < 0)
	{
		LOG("error : %s", strerror(errno));
		return -1;
	}
	return ret;
}

static int devserial_write(struct proto_IfaceIODevice* _this, const void * buffer, uint8_t size)
{
    assert(_this && buffer);
	proto_dev_serial_t* infos = _this->user;
	if (infos->fd < 0)
	{
		LOG("error : invalid descriptor");
		return -1;
	}
	LOG("writing %d bytes to fd/%d", size, infos->fd);

    while (size > 0) {
        int ret = write(infos->fd, buffer, size);
		if (ret < 0)
		{
            LOG("error : %s (fd=%d)", strerror(errno ), infos->fd);
			return -1;
		}
        else {
            buffer += ret; // octets qui restent à transmettre
            size -= ret;
        }
    }
    return 0;
}

int devserial_setFD(proto_Device_t _this, int fd) {
    assert(_this);

	if (fd < 0)
		return -1;

	// On ferme les ressources existantes
	devserial_close(_this);

// 20200924 SLI - Maybe not needed for android, in particular for USB layers. 
#ifdef ENABLE_SET_FD_MODE
    int ret = setFdMode(fd, 0, 0);
	if (ret < 0)
	{
		LOG("setFdMode error : aborted");
		return -1;
	}
#endif
    proto_dev_serial_t* infos = _this->user;
    infos->fd = fd;
    return 0;
}

int devserial_getFD(proto_Device_t _this) {

    return ((proto_dev_serial_t*) _this->user)->fd;

}

#else // if __linux__ not defined

static int devserial_open(struct proto_IfaceIODevice* _this, const char * szPath)
{
    UNUSED(_this);
    UNUSED(szPath);
    return -1;
}

static int devserial_close(struct proto_IfaceIODevice* _this)
{
    UNUSED(_this);
    return -1;
}

static int devserial_read(struct proto_IfaceIODevice* _this, void* buffer, uint8_t bufferSize, int16_t tout_ms)
{
    UNUSED(_this);
    UNUSED(buffer);
    UNUSED(bufferSize);
    UNUSED(tout_ms);
    return 0;
}

static int devserial_write(struct proto_IfaceIODevice* _this, const void * buffer, uint8_t size)
{
    UNUSED(_this);
    UNUSED(buffer);
    UNUSED(size);
    return 0;
}

int devserial_openFD(proto_Device_t _this, int fd) {
    UNUSED(_this);
    UNUSED(fd);
    return -1;
}


#endif // if __linux__ not defined

proto_Device_t devserial_create(void)
{
    /// allocation des structures et initialisation du pointeur
    proto_Device_t _this  = (proto_Device_t)malloc( sizeof(proto_IfaceIODevice_t) );
    _this->user = (proto_dev_serial_t*)malloc(sizeof(proto_dev_serial_t));
    devserial_init(_this);

    return _this;
}


void devserial_init(proto_Device_t _this)
{
    assert(_this);
	DEVIO_INIT(devserial, _this);

    proto_dev_serial_t* infos = _this->user;
    infos->fd = -1; // Initialisation à la valeur "invalide" retournée par l'open
}
