#include "protocomm-details.h"
#include <string.h>
#include <assert.h>
#include <time.h>

typedef struct Received_t {
    uint8_t wasReceived;
    proto_Command_t command;
    uint8_t args[proto_MAX_ARGS];
} Received_t;

// Le callback ne fait qu'écrire dans Received_t la trame reçue
static void master_callback(void* userdata, proto_Command_t command, uint8_t const* args) {
    Received_t* received = userdata;
    received->wasReceived = 1;
    received->command = command;
    // on fait une copie car Received_t a une durée de vie plus grande
    // que *args (args faisant référence à un buffer interne de proto_State_t)
    memcpy(received->args, args, proto_getArgsSize(command));
}

static Received_t waitAnswer(uint16_t timeout_ms, proto_Device_t device, void* iodata) {
    clock_t start = clock();
    Received_t received = { 0 };
    proto_State_t state = { 0 };
    proto_setReceiver(&state, master_callback, &received);
    do {
        if (proto_readBlob(&state, device, iodata))
            break; // si on a lu une trame complète, on s'arrête tout de suite
        
        // refaire tant que le temps écoulé (converti en millisecondes) est < à timeout_ms
    } while ((clock() - start) * 1000.0 / CLOCKS_PER_SEC < timeout_ms);
    return received;
}

void proto_closeDevice(proto_Device_t device, void* iodata) {
    assert(device != NULL);
    device->destroy(iodata);
}

proto_Status_t proto_cio_master_get(uint8_t register_, uint8_t* value, 
                    uint16_t timeout_ms, proto_Device_t device, void* iodata) {
    proto_writeFrame(proto_GET, &register_, device, iodata);
    Received_t received = waitAnswer(timeout_ms, device, iodata);
    
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

proto_Status_t proto_cio_master_set(uint8_t register_, uint8_t value,
                    uint16_t timeout_ms, proto_Device_t device, void* iodata) {
    uint8_t args[2] = { register_, value };
    proto_writeFrame(proto_SET, args, device, iodata);
    Received_t received = waitAnswer(timeout_ms, device, iodata);
    
    if (!received.wasReceived)
        return proto_TIMEOUT;
    else if (received.command == proto_NOTIF_BAD_CRC)
        return proto_INVALID_CRC;
    else {
        assert(received.command == proto_STATUS);
        return received.args[0];
    }
}
