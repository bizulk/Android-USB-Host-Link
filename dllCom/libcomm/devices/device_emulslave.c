
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
     uint16_t priv_masterStkSize; ///< Taille occupée dans la stack

    proto_hdle_t * slaveThis; ///!< Instance du slave
    union{
     proto_Frame_t frame; /// Acces par frame (lorsque le slave push)
     uint8_t buf[ sizeof(proto_Frame_t) ]; ///< Acces par octets (quane le master read)
    } priv_slaveStk;
     uint16_t priv_slaveStkSize; ///< Taille occupée dans la stack
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
static proto_Device_t devemulslave_t_fakeSlaveCreate(proto_Device_t masterthis);
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
static int devemulslave_callback(void* userdata, proto_Command_t command, uint8_t * args);

/******************************************************************************
 * FUNCTION
******************************************************************************/

uint8_t * LIBCOMM_EXPORT devemulslave_getRegisters(proto_Device_t this)
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
    proto_Device_t slavedev = devemulslave_t_fakeSlaveCreate(this);
    slave->slaveThis = proto_slave_create(slavedev, devemulslave_callback, slave);
}

static int  devemulslave_open(proto_Device_t this, const char * szPath)
{
    return 0;
}

static int devemulslave_close(proto_Device_t this)
{
    return 0;
}

static int devemulslave_read(proto_Device_t this, void* buf, uint8_t len, int16_t tout_ms)
{

    assert(this && buf);
    UNUSED(tout_ms);

    proto_dev_emulslave_t* slave = this->user;
    uint8_t nbDelayedBytes = slave->priv_slaveStkSize;

    // On prend le minimum entre bufferSize et nbDelayedBytes
    int nbRead = len < nbDelayedBytes ? len : nbDelayedBytes;
    // On copie les octets dans le buffer donné
    memcpy(buf, slave->priv_slaveStk.buf, nbRead);
    // On dépile les octets du buffer
    memmove(slave->priv_slaveStk.buf, slave->priv_slaveStk.buf + nbRead, nbDelayedBytes - nbRead);
    slave->priv_slaveStkSize -= nbRead;

    return nbRead;
}

static int devemulslave_write(proto_Device_t this, const void * buf, uint8_t len)
{
    assert(this && this->user);
    assert(buf);
    int ret = 0;

    // On push sur le buffer autant que possible
    proto_dev_emulslave_t* slave = this->user;
    uint8_t sizeLeft = sizeof(slave->priv_masterStk) - slave->priv_masterStkSize;

    // D'après la spec d'interface on peut tout écrire ou pas.
    int nbWrite = (len < sizeLeft) ? len : sizeLeft;
    if( nbWrite < len )
        return -1;

    // On copie les octets dans le buffer interne
    memcpy(slave->priv_masterStk.buf + slave->priv_masterStkSize,buf, nbWrite);
    slave->priv_masterStkSize += nbWrite;

    // On traite immédiatement la requête
    proto_slave_main(slave->slaveThis);

    return 0;
}

static int devemulslave_callback(void* userdata, proto_Command_t command, uint8_t * args) {

    int ret = 0;
    proto_dev_emulslave_t* this = userdata;
	switch (command) {
    case proto_CMD_SET: // quand le MASTER demande de changer une valeur
		if (args[0] < 20) {
			this->registers[args[0]] = args[1];
		} else
            ret = -1;
		break;
		
    case proto_CMD_GET: // quand le MASTER demande d'accéder à une valeur
		if (args[0] < 20)
            args[0] = this->registers[args[0]];
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
    return 0;
}
static int devemulslave_fake_close(proto_Device_t this)
{
    return 0;
}


static int devemulslave_fake_read(proto_Device_t this, void* buf, uint8_t len, int16_t tout_ms)
{
    assert(this && buf);
    UNUSED(tout_ms);

    proto_dev_emulslave_t* slave = this->user;
    uint8_t nbDelayedBytes = slave->priv_masterStkSize;

    // On prend le minimum entre bufferSize et nbDelayedBytes
    int nbRead = len < nbDelayedBytes ? len : nbDelayedBytes;
    // On copie les octets dans le buffer donné
    memcpy(buf, slave->priv_slaveStk.buf, nbRead);
    // On dépile les octets du buffer
    memmove(slave->priv_slaveStk.buf, slave->priv_slaveStk.buf + nbRead, nbDelayedBytes - nbRead);
    slave->priv_slaveStkSize -= nbRead;

    return nbRead;
}

static int devemulslave_fake_write(proto_Device_t this, const void * buf, uint8_t len)
{
    assert(this && this->user);
    assert(buf);

    // On push sur le buffer autant que possible
    proto_dev_emulslave_t* slave = this->user;
    uint8_t sizeLeft = sizeof(slave->priv_slaveStk) - slave->priv_slaveStkSize;

    // D'après la spec d'interface on peut tout écrire ou pas.
    int nbWrite = (len < sizeLeft) ? len : sizeLeft;
    if( nbWrite < len )
        return -1;

    // On copie les octets dans le buffer interne
    memcpy(slave->priv_slaveStk.buf + slave->priv_slaveStkSize,buf, nbWrite);
    slave->priv_slaveStkSize += nbWrite;

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
proto_Device_t devemulslave_t_fakeSlaveCreate(proto_Device_t masterthis)
{
    proto_Device_t this = (proto_Device_t)malloc(sizeof(proto_IfaceIODevice_t));
    DEVIO_INIT(devemulslave_fake, this);
    this->user = masterthis;
    return this;
}


void LIBCOMM_EXPORT devemulslave_setFlags(proto_Device_t this, uint8_t FLAGS)
{
    proto_dev_emulslave_t* slave = this->user;
    slave->flags = FLAGS;
}

void LIBCOMM_EXPORT devemulslave_getFlags(proto_Device_t this, uint8_t *FLAGS)
{
    proto_dev_emulslave_t* slave = this->user;
    *FLAGS = slave->flags;
}
