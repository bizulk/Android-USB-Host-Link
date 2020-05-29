
#include <stdlib.h>
#include <assert.h>
#include "proto_iodevice.h"
#include "device_serial.h"
#include <string.h>


/******************************************************************************
 * TYPES & VARIABLES
******************************************************************************/

#define STM32_FIFO_MAX_SIZE 40

/* Complément de données nécessaire à la structure
 * Stocke les octets reçus pour un accès en FIFO
 * dans une structure de données type ring buffer.
*/
typedef struct
{
    uint8_t buffer[STM32_FIFO_MAX_SIZE];
    uint8_t begin;
    uint8_t end;
} proto_dev_stm32_t;

static void push_byte(proto_dev_stm32_t* self, uint8_t value) {
    self->buffer[self->end] = value;
    ++self->end;
    if (self->end == STM32_FIFO_MAX_SIZE)
        self->end = 0;
}
static uint8_t pop_byte(proto_dev_stm32_t* self) {
    uint8_t value = self->buffer[self->begin];
    ++self->begin;
    if (self->begin == STM32_FIFO_MAX_SIZE)
        self->begin = 0;
    return value;
}


///
/// \brief devserial_init : même chose que create, mais avec une instance alloué sur la pile
/// \warning Ne pas appeler "destroy" pour ce cas
/// \param this Instance pre-alloué
///
static void devstm32_init(proto_Device_t this);


static void devstm32_destroy(proto_Device_t this)
{
    assert(this && this->user);

    /// Fermeture des ports (todo).
    ///
    /// Liberation des structures
    this->close(this);
    free(this->user);
    free(this);
}

// réinitialise la fifo
static int devstm32_open(struct proto_IfaceIODevice* this, const char * szPath)
{
    UNUSED(szPath);
    memset(this->user, 0, sizeof(proto_dev_stm32_t));
    return 0;
}

// ne fait rien
static int devstm32_close(struct proto_IfaceIODevice* this) {
    UNUSED(this);
    return 0;
}

static int devstm32_read(struct proto_IfaceIODevice* this, void* voidbuffer, uint8_t bufferSize, int16_t tout_ms)
{
    UNUSED(tout_ms);
    proto_dev_stm32_t* self = this->user;
    uint8_t* buffer = voidbuffer;

    uint8_t i = 0;

    while (self->begin != self->end) {
        if (i == bufferSize)
            break; // on a rempli le buffer
        buffer[i] = pop_byte(self);
        ++i;
    }

    return i;
}

static int devstm32_write(struct proto_IfaceIODevice* this, const void * buffer, uint8_t size)
{
    UNUSED(this);
    // TODO: Il faut ajouter ici l'envoi des octets
    // Et retourner 0 si OK, et -1 si erreur

    return -1;
}


proto_Device_t devstm32_create(void)
{
    /// allocation des structures et initialisation du pointeur
    proto_Device_t this  = (proto_Device_t)malloc( sizeof(proto_IfaceIODevice_t) );
    this->user = malloc(sizeof(proto_dev_stm32_t));
    devstm32_init(this);

    return this;
}


void devstm32_init(proto_Device_t this)
{
    assert(this);
    this->open = devstm32_open;
    this->close = devstm32_close;
    this->destroy = devstm32_destroy;
    this->read = devstm32_read;
    this->write = devstm32_write;

    devstm32_open(this, ""); // memset à 0 du userdata
}


void devstm32_pushBytes(proto_Device_t this, uint8_t* bytes, uint8_t nbBytes) {
    for (int i = 0; i < nbBytes; ++i)
        push_byte(this->user, bytes[i]);
}
