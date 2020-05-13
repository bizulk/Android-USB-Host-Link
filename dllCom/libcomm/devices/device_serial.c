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

/******************************************************************************
 * TYPES & VARIABLES
******************************************************************************/

/* Complément de données nécessaire à la structure */
typedef struct
{
    int toto;
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


static int  devserial_open(struct proto_IfaceIODevice* this, const char * szPath)
{
    return -1;
}

static int devserial_close(struct proto_IfaceIODevice* this)
{
    return -1;
}

static int devserial_read(struct proto_IfaceIODevice* this, void* buffer, uint8_t bufferSize, int16_t tout_ms)
{
    return 0;
}

static int devserial_write(struct proto_IfaceIODevice* this, const void * buffer, uint8_t size)
{
    return 0;
}


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
    memset(this->user, 0, sizeof(proto_dev_serial_t));
}
