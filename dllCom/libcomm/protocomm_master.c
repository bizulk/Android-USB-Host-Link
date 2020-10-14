
#include <string.h>
#include <assert.h>
#include <time.h>
#include <stdlib.h>

#include "protocomm_master.h"
#include "devices/device_serial.h"
#include "log.h"

/******************************************************************************
 * TYPES & VARIABLES
******************************************************************************/

/******************************************************************************
 * LOCAL PROTO
******************************************************************************/

static proto_Status_t proto_master_readFrame(proto_hdle_t * this, proto_frame_data_t * arg);

/******************************************************************************
 * FUNCTIONS
******************************************************************************/

///
/// \brief proto_master_create Creation d'une instance pour le master
/// \param iodevice Instance du device
/// \param callback
/// \param userdata
///
proto_hdle_t * proto_master_create(proto_Device_t iodevice)
{
    return proto_create(iodevice);
}

void proto_master_destroy(proto_hdle_t * this)
{
    return proto_destroy(this);
}

int proto_master_open(proto_hdle_t * this, const char * szPath)
{
    return proto_open(this, szPath);
}

int proto_master_close(proto_hdle_t * this)
{
    return proto_close(this);
}

proto_hdle_t * proto_cio_open(const char * szDev)
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

static proto_Status_t proto_master_readFrame(proto_hdle_t * this, proto_frame_data_t * arg)
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
            // On fait juste de la validation de contenu a ce stage
            switch (cmd) {
            case proto_CMD_REPLY:
                // rien a faire
                break;
            // Rappel : la trame est bonne pour nous, c'est le slave qui nous retourne une erreur de traitement
            case proto_CMD_ERR_CRC:
                ret = proto_PEER_ERR_CRC;
                break;
            case proto_CMD_ERR_ARG:
                // Les arguments  de retour sont déjà positionnés
                ret = proto_INVALID_ARG;
                break;
            default:
                ret = proto_ERR_PROTO;
                break;
            }
            break;
         // Il y a eu un problème de transmission slave -> master
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

    proto_frame_data_t data = { .req.reg = register_, .req.value = 0};
    proto_Status_t ret = 0;


    if( proto_writeFrame(this, proto_CMD_GET, &data) != 0)
    {
    	LOG("error : proto_writeFrame (proto_CMD_GET)");
        return proto_ERR_SYS;
    }
    ret = proto_master_readFrame(this, &data);
    if( ret == proto_NO_ERROR)
    {
        *value = data.reg_value;
    }
    return ret;
}

proto_Status_t proto_master_set(proto_hdle_t * this, uint8_t register_, uint8_t value) {

    proto_frame_data_t data = { .req.reg = register_, .req.value = value};
    proto_Status_t ret = 0;

    LOG("info : proto_master_set");

    if( proto_writeFrame(this, proto_CMD_SET, &data) != 0)
    {
    	LOG("error : proto_writeFrame (proto_CMD_SET)");
        return proto_ERR_SYS;
    }
    ret = proto_master_readFrame(this, &data);
    return ret;
}
