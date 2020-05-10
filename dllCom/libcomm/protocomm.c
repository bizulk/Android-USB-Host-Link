
#include <string.h>
#include <assert.h>
#include <time.h>
#include <stdlib.h>

#include "protocomm.h"
#include "devices/device_serial.h"

/******************************************************************************
 * TYPES & VARIABLES
******************************************************************************/

typedef struct Received_t {
    uint8_t wasReceived;
    proto_Command_t command;
    uint8_t args[proto_MAX_ARGS];
} Received_t;

/******************************************************************************
 * LOCAL PROTO
******************************************************************************/

///
/// \brief master_callback callback de reception d'une trame
/// \param userdata
/// \param command la requete de réponse
/// \param args
///
static void master_callback(void* userdata, proto_Command_t command, const uint8_t * args);

/******************************************************************************
 * FUNCTIONS
******************************************************************************/


proto_hdle_t * proto_cio_open(const char * szDev, uint16_t timeout_ms)
{
    assert(szDev);
    int ret = 0;
    proto_Device_t iodevice = devserial_create();
    proto_hdle_t * this = proto_create(iodevice);
    ret = this->priv_iodevice->open(this->priv_iodevice, szDev);
    // Si ça n'a pas marché on annule tout
    if(ret != 0)
    {
        proto_destroy(this);
        this = NULL;
    }
    return this;
}

// Le callback ne fait qu'écrire dans Received_t la trame reçue
static void master_callback(void* userdata, proto_Command_t command, const uint8_t * args) {
    Received_t* received = userdata;
    received->wasReceived = 1;
    received->command = command;
    // on fait une copie car Received_t a une durée de vie plus grande
    // que *args (args faisant référence à un buffer interne de proto_State_t)
    memcpy(received->args, args, proto_getArgsSize(command));
}

static Received_t waitAnswer(proto_hdle_t * this, uint16_t timeout_ms) {
    clock_t start = clock();
    // On utilise une instance local de userdata
    Received_t received = { 0 };
    proto_setReceiver(this, master_callback, &received);
    do {
        if (proto_readFrame(this))
            break; // si on a lu une trame complète, on s'arrête tout de suite
        
        // refaire tant que le temps écoulé (converti en millisecondes) est < à timeout_ms
    } while ((clock() - start) * 1000.0 / CLOCKS_PER_SEC < timeout_ms);
    return received;
}

proto_Status_t proto_cio_master_get(proto_hdle_t * this, uint8_t register_, uint8_t* value) {

    proto_frame_arg_t arg = { .reg = register_, .value = 0};
    proto_writeFrame(this, proto_GET, (void*)&arg);
    Received_t received = waitAnswer(this, 1000);
    
    if (!received.wasReceived)
        return proto_TIMEOUT;
    else if (received.command == proto_STATUS)
        return received.args[0];
    else if (received.command == proto_NOTIF_BAD_CRC)
        return proto_INVALID_CRC;
    else {
        assert(received.command == proto_REPLY);
        *value = received.args[0];
        return proto_NO_ERROR;
    }
}

proto_Status_t proto_cio_master_set(proto_hdle_t * this, uint8_t register_, uint8_t value) {
    void * iodata = NULL; // TODO
    proto_frame_arg_t arg = { .reg = register_, .value = value};
    proto_writeFrame(this, proto_SET, (void*)&arg);
    Received_t received = waitAnswer(this, timeout_ms);
    
    if (!received.wasReceived)
        return proto_TIMEOUT;
    else if (received.command == proto_NOTIF_BAD_CRC)
        return proto_INVALID_CRC;
    else {
        assert(received.command == proto_STATUS);
        return received.args[0];
    }
}
