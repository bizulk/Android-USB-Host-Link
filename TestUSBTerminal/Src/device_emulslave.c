
#include <string.h>
#include <assert.h>
#include <stdlib.h>

/// Implementation du device emulslave
///
///
/// quand on appelle write() du device,/// Les octets des trames renvoyées sont
/// stockées dans une queue interne (priv_delayedBytes). Appeler read() du device
/// fait "popper" les octets de cette queue. (PS : la queue est First In, First Out).
/// On peut accéder directement aux registres internes de l'émulateur avec
///     proto_Data_EmulSlave_t devicedata;
///     proto_initData_EmulSlave(&devicedata);
///     ...
///     printf("%d\n", (int)devicedata.registers[3]); // affiche la valeur dans le registre 3
///
///
#include "device_emulslave.h"
#include "protocomm_slave.h"

/// Structure complémentaire pour l'émulation de l'esclave.
/// Les attributs préfixés priv_ ne doivent pas être accédés (violation de l'encapsulation).
typedef struct proto_dev_emulslave {
    uint8_t registers[EMULSLAVE_NB_REGS]; ///< Les registres en accès du slave
    uint8_t flags; ///< Flags qui contrôle le comportement du device \sa emulslave_flags

    union{
     proto_Frame_t frame; /// Acces par frame (lorsque le slave push)
     uint8_t buf[ sizeof(proto_Frame_t) ]; ///< Acces par octets (quane le master read)
    } priv_masterStk;
     uint16_t priv_usMasterStkSize; ///< Taille occupée dans la stack

    proto_hdle_t * slaveThis; ///!< Instance du slave
    union{
     proto_Frame_t frame; /// Acces par frame (lorsque le slave push)
     uint8_t buf[ sizeof(proto_Frame_t) ]; ///< Acces par octets (quane le master read)
    } priv_slaveStk;
     uint16_t priv_usSlaveStkSize; ///< Taille occupée dans la stack
} proto_dev_emulslave_t;

///
/// \brief même chose que create, mais avec une instance alloué sur la pile
/// \warning Ne pas appeler "destroy" pour ce cas
/// \param this Instance pre-alloué
///
static void devemulslave_init(proto_Device_t this);

///
/// \brief devemulslave_open fonction open pour le proto
/// \param this
/// \param szPath
/// \return
///
static int devemulslave_open(proto_Device_t this, const char * szPath);
static int devemulslave_close(proto_Device_t this);
static int devemulslave_read(proto_Device_t this, void* buf, uint8_t len, int16_t tout_ms);
static int devemulslave_write(proto_Device_t this, const void * buf, uint8_t len);
static void devemulslave_destroy(proto_Device_t this);

/// Fonctions internes pour l'émulation du slave du slave
static proto_Device_t devemulslave_fakeSlaveCreate(proto_Device_t masterthis);
static int devemulslave_fake_open(proto_Device_t this, const char * szPath);
static int devemulslave_fake_close(proto_Device_t this);
/// La fonction va dépiler les trames dans la stack master
static int devemulslave_fake_read(proto_Device_t this, void* buf, uint8_t len, int16_t tout_ms);
/// La fonction va empiler les trames dans la stack slave
static int devemulslave_fake_write(proto_Device_t this, const void * buf, uint8_t len);
static void devemulslave_fake_destroy(proto_Device_t this);

///
/// \brief emulslave_callback Callback de réception d'une requete master
/// \param userdata : les registres de données
/// \param command commande reçu
/// \param[inout] args argument d'entrée
/// \return
///
static int devemulslave_callback(void* userdata, proto_Command_t command, proto_frame_data_t* args);

///
/// \brief devemulslave_isFrame fonction utilitaire qui vérifie que l'on a une trame dans notre bloc
/// \param pFrame la trame supposée
/// \param pFrame la taille de la trame que l'on a
/// \return 0 on a bien une trame, sinon pas de trame identifiée
///
/// On considère que l'on a une trame si l'on a SOF, une commande valide et une taille suffisante
///
static int devemulslave_isFrame(proto_Frame_t *pFrame, uint8_t len);

/******************************************************************************
 * FUNCTION
******************************************************************************/

uint8_t * devemulslave_getRegisters(proto_Device_t this)
{
    assert(this && this->user);
    proto_dev_emulslave_t* slave = this->user;

    return slave->registers;
}

proto_Device_t devemulslave_create(void)
{
    /// allocation des structures et initialisation du pointeur
    proto_Device_t this  = (proto_Device_t)malloc( sizeof(proto_IfaceIODevice_t) );
    this->user = (proto_dev_emulslave_t*)malloc(sizeof(proto_dev_emulslave_t));
    devemulslave_init(this);

    return this;
}

void devemulslave_destroy(proto_Device_t this)
{
    assert(this && this->user);
    proto_dev_emulslave_t* slave = this->user;
    proto_destroy(slave->slaveThis);
    // Liberation des structures sous jacentes
    this->close(this);
    free(this->user);
    free(this);
}

void devemulslave_init(proto_Device_t this)
{
    assert(this);
    DEVIO_INIT(devemulslave, this);

    proto_dev_emulslave_t* slave = (proto_dev_emulslave_t*)this->user;
    memset(slave, 0, sizeof(proto_dev_emulslave_t));
    // Creation de l'instance de protocole slave
    proto_Device_t slavedev = devemulslave_fakeSlaveCreate(this);
    slave->slaveThis = proto_slave_create(slavedev, devemulslave_callback, slave);
}

static int  devemulslave_open(proto_Device_t this, const char * szPath)
{
    UNUSED(this);
    UNUSED(szPath);
    return 0;
}

static int devemulslave_close(proto_Device_t this)
{
    UNUSED(this);
    return 0;
}

static int devemulslave_read(proto_Device_t this, void* buf, uint8_t len, int16_t tout_ms)
{

    assert(this && buf);
    UNUSED(tout_ms);

    proto_dev_emulslave_t* slave = this->user;
    uint16_t nbDelayedBytes = slave->priv_usSlaveStkSize;

    // On prend le minimum entre bufferSize et nbDelayedBytes
    int nbRead = len < nbDelayedBytes ? len : nbDelayedBytes;
    // On copie les octets dans le buffer donné
    memcpy(buf, slave->priv_slaveStk.buf, nbRead);
    // On dépile les octets du buffer
    memmove(slave->priv_slaveStk.buf, slave->priv_slaveStk.buf + nbRead, nbDelayedBytes - nbRead);
    slave->priv_usSlaveStkSize -= nbRead;

    return nbRead;
}

static int devemulslave_isFrame(proto_Frame_t *pFrame, uint8_t len)
{
    if( pFrame->startOfFrame != proto_START_OF_FRAME)
        return -1;
    if( len < proto_COMMAND_OFFSET )
        return -1;
    if( !(pFrame->command < proto_LAST))
        return -1;

    uint8_t framelen = proto_ARGS_OFFSET + proto_getArgsSize(pFrame->command);
    if( len != framelen)
        return -1;
    return 0;
}

static int devemulslave_write(proto_Device_t this, const void * buf, uint8_t len)
{
    assert(this && this->user);
    assert(buf);

    // On push sur le buffer autant que possible
    proto_dev_emulslave_t* slave = this->user;
    uint16_t sizeLeft = sizeof(slave->priv_masterStk) - slave->priv_usMasterStkSize;

    // D'après la spec d'interface on peut tout écrire ou pas.
    int nbWrite = (len < sizeLeft) ? len : sizeLeft;
    if( nbWrite < len )
        return -1;

    // On copie les octets dans le buffer interne
    memcpy(slave->priv_masterStk.buf + slave->priv_usMasterStkSize,buf, nbWrite);
    slave->priv_usMasterStkSize += nbWrite;

    // Test : on manipule la trame selon le flag positionne
    if( (slave->flags & EMULSLAVE_FLAG_MASTER_BADCRC) &&
        (devemulslave_isFrame(&slave->priv_masterStk.frame, (uint8_t)slave->priv_usMasterStkSize)==0) )
    {
        // on se contente d'incrémenter le CRC
        slave->priv_masterStk.frame.crc8++;
    }

    // On traite immédiatement la requête
    proto_slave_main(slave->slaveThis);

    return 0;
}

static int devemulslave_callback(void* userdata, proto_Command_t command, proto_frame_data_t* args) {

    int ret = 0;
    proto_dev_emulslave_t* this = userdata;
	switch (command) {
    case proto_CMD_SET: // quand le MASTER demande de changer une valeur
        if (args->req.reg < EMULSLAVE_NB_REGS) {
            this->registers[args->req.reg] = args->req.value;
        } else
            ret = -1;
        break;
		
    case proto_CMD_GET: // quand le MASTER demande d'accéder à une valeur
        if (args->req.reg < EMULSLAVE_NB_REGS)
            args->reg_value = this->registers[args->req.reg];
        else
            ret = -1;
        break;
		
    default:
        ret = -1;
        break;
    }
    return ret;
}

static int devemulslave_fake_open(proto_Device_t this, const char * szPath)
{
    UNUSED(this);
    UNUSED(szPath);
    return 0;
}
static int devemulslave_fake_close(proto_Device_t this)
{
    UNUSED(this);
    return 0;
}


static int devemulslave_fake_read(proto_Device_t this, void* buf, uint8_t len, int16_t tout_ms)
{
    assert(this && buf);
    UNUSED(tout_ms);

    proto_Device_t masterdev_base = (proto_Device_t)this->user;
    proto_dev_emulslave_t * masterdev = (proto_dev_emulslave_t *)masterdev_base->user;
    uint16_t usStkSize = masterdev->priv_usMasterStkSize;

    // On prend le minimum entre bufferSize et nbDelayedBytes
    int nbRead = len < usStkSize ? len : usStkSize;
    // On copie les octets dans le buffer donné
    memcpy(buf, masterdev->priv_masterStk.buf, nbRead);
    // On dépile les octets du buffer
    memmove(masterdev->priv_slaveStk.buf, masterdev->priv_masterStk.buf + nbRead, usStkSize - nbRead);
    masterdev->priv_usMasterStkSize -= nbRead;

    return nbRead;
}

static int devemulslave_fake_write(proto_Device_t this, const void * buf, uint8_t len)
{
    assert(this && this->user);
    assert(buf);

    // On push sur le buffer autant que possible
    proto_Device_t masterdev_base = (proto_Device_t)this->user;
    proto_dev_emulslave_t * masterdev = (proto_dev_emulslave_t *)masterdev_base->user;

    uint16_t sizeLeft = sizeof(masterdev->priv_slaveStk) - masterdev->priv_usSlaveStkSize;

    // D'après la spec d'interface on peut tout écrire ou pas.
    int nbWrite = (len < sizeLeft) ? len : sizeLeft;
    if( nbWrite < len )
        return -1;

    // On copie les octets dans le buffer interne
    memcpy(masterdev->priv_slaveStk.buf + masterdev->priv_usSlaveStkSize,buf, nbWrite);
    masterdev->priv_usSlaveStkSize += nbWrite;

    return 0;
}

static void devemulslave_fake_destroy(proto_Device_t this)
{
    assert(this);
    // On appelle l'effacement du device
    // Liberation des structures sous jacentes
    this->close(this);
    // Cette structure est imbriquée : user pointe vers la structure du master, qui se charge de sa propre destruction
    //free(this->user);
    free(this);
}
proto_Device_t devemulslave_fakeSlaveCreate(proto_Device_t masterthis)
{
    proto_Device_t this = (proto_Device_t)malloc(sizeof(proto_IfaceIODevice_t));
    DEVIO_INIT(devemulslave_fake, this);
    this->user = masterthis;
    return this;
}


void devemulslave_setFlags(proto_Device_t this, uint8_t FLAGS)
{
    proto_dev_emulslave_t* slave = this->user;
    slave->flags = FLAGS;
}

void devemulslave_getFlags(proto_Device_t this, uint8_t *FLAGS)
{
    proto_dev_emulslave_t* slave = this->user;
    *FLAGS = slave->flags;
}
