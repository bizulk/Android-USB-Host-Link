#include "protocomm_ll.h"

#include <assert.h>
#include <string.h>
#include <malloc.h>

/******************************************************************************
 * TYPES & CONSTANT & VARIABLE
******************************************************************************/

/// Statut en résultat de proto_interpretByte
typedef enum proto_ResultStatus_t {
    proto_WAITING,   /// La trame n'est pas finie
    proto_COMPLETED, /// On a lu une trame correctement
    proto_REFUSED    /// On a lu une trame mais il y a eu une erreur (exemple : CRC)
} proto_ResultStatus_t;

/******************************************************************************
 * LOCAL PROTOTYPES
******************************************************************************/

static uint8_t getCRC(uint8_t const* data, uint8_t size);
static uint8_t getFrameCRC(proto_Frame_t* pframe);

/// Cette fonction est responsable de créer la trame de notification
/// d'erreur car c'est elle qui a le plus d'informations sur le CRC.
static proto_ResultStatus_t proto_decodeFrame(proto_hdle_t* this, proto_Command_t * cmd, proto_frame_arg_t *arg);

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

void proto_setReceiver(proto_hdle_t* this, proto_OnReception_t callback, void* userdata) {
    assert(this != NULL);
    this->priv_callback = callback;
    this->priv_userdata = userdata;
}

int proto_writeFrame(proto_hdle_t* this, proto_Command_t command, uint8_t const* args) {
    proto_Frame_t frame = {0};
    uint8_t nbBytes = proto_makeFrame(&frame, command, args);
    return this->priv_iodevice->write(this->priv_iodevice, &frame, nbBytes);
}

uint8_t proto_makeFrame(proto_Frame_t* frame, proto_Command_t command, uint8_t const* args) {
    assert(frame != NULL);
    assert(args != NULL);
    uint8_t argSize = proto_getArgsSize(command);

    frame->startOfFrame = proto_START_OF_FRAME;
    frame->command = command;
    memset(&frame->arg, 0, sizeof(frame->arg));
    memcpy(&frame->arg, args, argSize); // on copie les arguments
    frame->crc8 = getFrameCRC(frame);
    return proto_ARGS_OFFSET + argSize;
}

int proto_readFrame(proto_hdle_t* this) {
    /* On se contente de lire une trame ou ce qu'il reste à en lire, pour éviter des tout
     * Par contre on ne peut utiliser la trame privée : la réception peut être externalisé
    */
    uint8_t buf[sizeof(proto_Frame_t)] = {0};
    uint8_t len = (sizeof(this->priv_frame)) - this->priv_nbBytes;
    uint8_t nbRead = this->priv_iodevice->read(this->priv_iodevice, buf, len);
    if( proto_pushToFrame(this, (uint8_t*)&this->priv_frame, nbRead) < nbRead
}

// Découpage du buffer d'entrée en trame
// Retourne : nombre d'octet lus :
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
            return len;
    }

    // Copie des octets (au passage le SOF)
    while( (this->priv_nbBytes < proto_ARGS_OFFSET) && (cursor<len) )
    {
        pcFrame[this->priv_nbBytes++] = buf[cursor++];
    }
    if( cursor == len )
       return len;
    framelen = proto_ARGS_OFFSET + proto_getArgsSize(this->priv_frame.command);
    while( (this->priv_nbBytes < framelen) && (cursor<len) )
    {
        pcFrame[this->priv_nbBytes++] = buf[cursor++];
    }
#if 0
    if (status == proto_COMPLETED) {
        this->priv_callback(
            this->priv_userdata,
            this->priv_frame.command,
            (uint8_t*)&this->priv_frame.arg);
        ++nbFrameEnd;
    }
#endif
    // Si trame terminée : on retourne ce qui a été consommée, sinon ce qui nous reste (en valeur négative)
    return (this->priv_nbBytes == framelen) ? cursor : (this->priv_nbBytes-framelen);
}

// Valisation du CRC, de la partie donnée
proto_ResultStatus_t proto_decodeFrame(proto_hdle_t* this, proto_Command_t * cmd, proto_frame_arg_t *arg)
{
    assert( cmd && arg);
    // TODO remplacer par un flag interne
    if (this->priv_nbBytes < proto_ARGS_OFFSET)
    {
        return proto_WAITING;
    }
    else if (this->priv_nbBytes < proto_ARGS_OFFSET + proto_getArgsSize(this->priv_frame.command) )
    {
        return proto_WAITING;
    }
    else if(getFrameCRC(&this->priv_frame) != this->priv_frame.crc8)
    {
        return proto_REFUSED;
    }

    *cmd = this->priv_frame.command;
    *arg = this->priv_frame.arg;

    // réinitialisation du curseur compteur
    this->priv_nbBytes = 0;

    return proto_COMPLETED;

    // La trame n'est pas encore complète, on a juste accès à l'octet
    // de commande, donc on peut connaître le nombre d'arguments attendus.
    if (this->priv_nbBytes >= proto_ARGS_OFFSET) {
        // si on a reçu l'octet de commande
        uint8_t nbArgs = proto_getArgsSize(this->priv_frame.command);
        if (this->priv_nbBytes == proto_ARGS_OFFSET + nbArgs) {

            // si on a reçu tous les octets d'arguments, on passe les
            // test de validation

            // on calcule le CRC sans compter le CRC reçu ni le Header
            uint8_t crcCalc = getFrameCRC(&this->priv_frame);
            if (crcCalc != this->priv_frame.crc8) {
#if 0
                // si le CRC ne correspond pas, on remplace la trame
                // actuelle par une notification de type BAD_CRC
                uint8_t args[2] = { this->priv_frame.crc8, crcCalc };
                this->priv_nbBytes = proto_makeFrame(&this->priv_frame, proto_NOTIF_BAD_CRC, args);
#endif
                return proto_REFUSED;
            }
            // une trame a été reçue en entier
        }
    }
    return proto_WAITING; // la trame n'a pas été reçue en entier
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

static uint8_t getFrameCRC(proto_Frame_t* pFrame) {
    uint8_t* data = (uint8_t*)pFrame;
    return getCRC(data + proto_COMMAND_OFFSET,
                  1 + proto_getArgsSize(data[proto_COMMAND_OFFSET]));
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
