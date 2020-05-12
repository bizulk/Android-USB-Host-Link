
#include <string.h>
#include <assert.h>
#include <time.h>
#include <stdlib.h>

#include "protocomm_master.h"
#include "devices/device_serial.h"

/******************************************************************************
 * TYPES & VARIABLES
******************************************************************************/

/******************************************************************************
 * LOCAL PROTO
******************************************************************************/

static proto_Status_t proto_master_readFrame(proto_hdle_t * this, proto_frame_arg_t * arg);

/******************************************************************************
 * FUNCTIONS
******************************************************************************/

///
/// \brief proto_master_create Creation d'une instance pour le master
/// \param iodevice Instance du device
/// \param callback
/// \param userdata
///
proto_hdle_t * LIBCOMM_EXPORT proto_master_create(proto_Device_t iodevice)
{
    return proto_create(iodevice);
}


int LIBCOMM_EXPORT proto_master_open(proto_hdle_t * this, const char * szPath)
{
    return this->priv_iodevice->open(this->priv_iodevice, szPath);
}

int LIBCOMM_EXPORT proto_master_close(proto_hdle_t * this)
{
    return this->priv_iodevice->close(this->priv_iodevice);
}

proto_hdle_t * LIBCOMM_EXPORT proto_cio_open(const char * szDev)
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

static proto_Status_t proto_master_readFrame(proto_hdle_t * this, proto_frame_arg_t * arg)
{
    proto_Status_t ret = 0;
    proto_Command_t cmd = 0;
    clock_t start = clock();
    // On travaille par scrutation, l'attente est bloquante et active
    do {
        ret = proto_readFrame(this, 0);
        if ( (ret == proto_NO_ERROR)||(proto_ERR_SYS) )
        {
            break;
        }
        // refaire tant que le temps écoulé (converti en millisecondes) est < à timeout_ms
    } while (((clock() - start) * 1000.0 / CLOCKS_PER_SEC) < PROTO_FRAME_RECV_TOUT_MS);

    // traitement complémentaire si ok, sinon on retourne l'erreur
    if(ret == proto_NO_ERROR)
    {
        switch( proto_decodeFrame(this, &cmd, arg))
        {
        case proto_COMPLETED:
            // On fait juste de la validation de contenu
            switch (cmd) {
            case proto_CMD_REPLY:
                // rien a faire
                break;
            case proto_CMD_ERR_ARG:
            case proto_CMD_ERR_CRC:
                // Les arguments sont positionnés
                ret = proto_INVALID_ARG;
                break;
            default:
                ret = proto_ERR_PROTO;
                break;
            }
            break;
        case proto_REFUSED:
            ret = proto_ERR_CRC;
            break;
        case proto_WAITING:
            // impossible ici
            assert(0);
            break;
        }
    }
    return ret;
}

proto_Status_t proto_master_get(proto_hdle_t * this, uint8_t register_, uint8_t* value) {

    proto_frame_arg_t arg = { .reg = register_, .value = 0};
    proto_Status_t ret = 0;


    if( proto_writeFrame(this, proto_CMD_GET, (void*)&arg) != 0)
    {
        return proto_ERR_SYS;
    }
    ret = proto_master_readFrame(this, &arg);
    if( ret == proto_NO_ERROR)
    {
        *value = arg.value;
    }
    return ret;
}

proto_Status_t proto_master_set(proto_hdle_t * this, uint8_t register_, uint8_t value) {

    proto_frame_arg_t arg = { .reg = register_, .value = value};
    proto_Status_t ret = 0;
    proto_Command_t cmd = 0;

    if( proto_writeFrame(this, proto_CMD_SET, (void*)&arg) != 0)
    {
        return proto_ERR_SYS;
    }
    ret = proto_master_readFrame(this, &arg);
    return ret;
}
