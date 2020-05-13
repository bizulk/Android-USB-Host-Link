#include "protocomm_ll.h"

#include <assert.h>
#include <string.h>
#include <malloc.h>

/******************************************************************************
 * TYPES & CONSTANT & VARIABLE
******************************************************************************/

/******************************************************************************
 * LOCAL PROTOTYPES
******************************************************************************/

static uint8_t getCRC(uint8_t const* data, uint8_t size);
static uint8_t getFrameCRC(const proto_Frame_t *pframe);



/******************************************************************************
 * FUNCTIONS
******************************************************************************/

proto_hdle_t * proto_create(proto_Device_t iodevice)
{
   // Io device peut être null
    // Dans ce cas on utilisera le proto seulement pour l'interprétation : on se charge de la transmission
    proto_hdle_t * this = (proto_hdle_t *)malloc(sizeof(proto_hdle_t));
    proto_init(this, iodevice);
    return this;
}

void proto_init(proto_hdle_t * this, proto_Device_t iodevice)
{
    memset(this, 0 ,sizeof(*this));
    this->priv_iodevice = iodevice;
}


void proto_destroy(proto_hdle_t * this)
{
    if( this->priv_iodevice)
        this->priv_iodevice->destroy(this->priv_iodevice);
    free(this);
}

int proto_open(proto_hdle_t * this, const char * szPath)
{
    assert( this && this->priv_iodevice);
    return this->priv_iodevice->open(this->priv_iodevice, szPath);
}

int proto_close(proto_hdle_t * this)
{
    assert( this && this->priv_iodevice);
    return this->priv_iodevice->close(this->priv_iodevice);
}

void proto_setReceiver(proto_hdle_t* this, proto_OnReception_t callback, void* userdata) {
    assert(this != NULL);
    this->priv_callback = callback;
    this->priv_userdata = userdata;
}

int proto_writeFrame(proto_hdle_t* this, proto_Command_t command, uint8_t const* args) {
    uint8_t nbBytes = proto_makeFrame(&this->priv_frame, command, args);
    return this->priv_iodevice->write(this->priv_iodevice, &this->priv_frame, nbBytes);
}

uint8_t proto_makeFrame(proto_Frame_t* frame, proto_Command_t command, uint8_t const* args) {
    assert(frame != NULL);
    assert(args != NULL);
    uint8_t argSize = proto_getArgsSize(command);

    frame->startOfFrame = proto_START_OF_FRAME;
    frame->command = command;
    memset(&frame->data, 0, sizeof(frame->data));
    memcpy(&frame->data, args, argSize); // on copie les arguments
    frame->crc8 = getFrameCRC(frame);
    return proto_ARGS_OFFSET + argSize;
}

proto_Status_t proto_readFrame(proto_hdle_t* this, int16_t tout_ms) {
    /* on ne peut utiliser la trame privée de this : la réception peut être externalisé
    */
    uint8_t buf[sizeof(proto_Frame_t)] = {0};
    uint8_t len =0;
    int nbRead = 0;
    int ret = 0;

    len = (sizeof(this->priv_frame)) - this->priv_nbBytes;
    nbRead = this->priv_iodevice->read(this->priv_iodevice, buf, len, tout_ms);
    if (nbRead > 0)
    {
        if( proto_pushToFrame(this, buf, nbRead) >= 0 )
        {
            ret = proto_NO_ERROR;
        }
        else // On n'a pas réussi à tout lire dans le délai
        {
            ret = proto_TIMEOUT;
        }
    }
    else if( nbRead < 0)
    {
        ret = proto_ERR_SYS;
    }
    else if( nbRead == 0)
    {
        ret = proto_TIMEOUT;
    }

    return ret;
}

int proto_pushToFrame(proto_hdle_t* this, const uint8_t * buf, uint32_t len) {
    assert(this != NULL && buf != NULL && len>0);
    uint32_t cursor = 0;
    uint8_t * pcFrame = (uint8_t*)&this->priv_frame;
    uint8_t framelen = 0;

    // Synchronisation sur le SOF
    if (this->priv_nbBytes == 0)
    {
        for( cursor = 0; cursor < len; cursor++ )
        {
            if( buf[cursor] == proto_START_OF_FRAME)
                break;
        }
        if(cursor == len)
            return -1;
    }

    // Copie des octets (au passage le SOF)
    while( (this->priv_nbBytes < proto_ARGS_OFFSET) && (cursor<len) )
    {
        pcFrame[this->priv_nbBytes++] = buf[cursor++];
    }
    if( cursor == len )
       return len;
    // Calcul de la taille complete de la trame
    framelen = proto_ARGS_OFFSET + proto_getArgsSize(this->priv_frame.command);
    while( (this->priv_nbBytes < framelen) && (cursor<len) )
    {
        pcFrame[this->priv_nbBytes++] = buf[cursor++];
    }

    // Si trame terminée : on retourne ce qui a été consommée, sinon -1
    return (this->priv_nbBytes == framelen) ? cursor : -1;
}

// Valisation du CRC, de la partie donnée
proto_DecodeStatus_t proto_decodeFrame(proto_hdle_t* this, proto_Command_t * cmd, proto_frame_data_t *arg)
{
    uint8_t crc8 = 0;
    assert( this && cmd && arg);
    if (this->priv_nbBytes < proto_ARGS_OFFSET)
    {
        return proto_WAITING;
    }
    else if (this->priv_nbBytes < (proto_ARGS_OFFSET + proto_getArgsSize(this->priv_frame.command)) )
    {
        return proto_WAITING;
    }
    else
    {
        crc8 = getFrameCRC(&this->priv_frame);
        if( crc8 != this->priv_frame.crc8 )
        {
            *cmd = proto_CMD_ERR_CRC;
            arg->crcerr[0] = this->priv_frame.crc8;
            arg->crcerr[1] = crc8;
            return proto_REFUSED;
        }
    }

    *cmd = this->priv_frame.command;
    *arg = this->priv_frame.data;

    // réinitialisation du curseur compteur pour permettre la réception de la prochaine trame
    this->priv_nbBytes = 0;
    return proto_COMPLETED;
}

static uint8_t getCRC(uint8_t const* data, uint8_t size) {
    // plus d'info : https://en.wikipedia.org/wiki/Cyclic_redundancy_check#Computation
    // implémentation de base : https://stackoverflow.com/a/51777726
    unsigned crc = 0; // l'accumulateur pour les divisions
    while (size--) {
        crc ^= *data++; // on prend un nouvel octet de la séquence
        for (unsigned k = 0; k < 8; k++) // pour chacun des 8 bits, dans l'ordre:
            crc = crc & 0x80 ? // si le bit "actuel" est 1
                    (crc << 1) ^ 0x31 : // on applique la division polynomiale
                    crc << 1; // sinon on passe au bit suivant
    }
    return crc & 0xff; // on ne garde que les 8 bits de poids faibles
}

static uint8_t getFrameCRC(const proto_Frame_t* pFrame) {
    return getCRC(&pFrame->command,
                  sizeof(pFrame->command) + proto_getArgsSize(pFrame->command));
}

uint8_t proto_getArgsSize(proto_Command_t cmd) {
    static uint8_t lut[proto_LAST] = {
        2, // proto_SET     : [REGISTRE] [VALEUR]
        1, // proto_GET     : [REGISTRE]
        1, // proto_REPLY   : [VALEUR]
        1, // proto_STATUS  : [proto_Status]
        2  // proto_NOTIF_BAD_CRC : [RECU] [CALCULE]
    };
    uint8_t nbArgs = cmd < proto_LAST ? lut[cmd] : 0;
    assert(nbArgs <= proto_MAX_ARGS);
    return nbArgs;
}
