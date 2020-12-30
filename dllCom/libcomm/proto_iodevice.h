#ifndef IODEVICE_H
#define IODEVICE_H

#ifdef __cplusplus
extern "C" {
#endif

/// Implementation d'un IO Device pour découpler la couche protocole de la liaison.
/// proto_Device_t est un pointeur vers un proto_IfaceIODevice.
/// Chaque instance d'un proto_IfaceIODevice représente un "device".
/// Un device permet d'envoyer et récupérer des octets.
/// Un device a trois états de vie : Fermé, Ouvert et Détruit.
/// Fermé : état après la création. Aucun octet ne peut être lu ni envoyé.
/// Ouvert : état depuis lequel on reçoit et envoit des octets.
/// Détruit : le device est désalloué, on ne doit plus l'utiliser !
/// On passe de Fermé à Ouvert en appelant monDevice->open(monDevice, ...)
/// On passe d'Ouvert à Fermé en appelant monDevice->close(monDevice)
/// On passe à l'état Détruit quand on fait monDevice->destroy(monDevice)
///
/// Un device peut être créé en appelant les fonctions devXXX_create()
/// disponibles dans les headers "devices/device_XXX.h"


#include <stdint.h>
#include <stddef.h>
#include "libcomm_global.h"

/// Enumerate all supported device
/// Some of them are available only for some platforms
typedef enum
{
	PROTO_DEV_EMULSLAVE,
	PROTO_DEV_SERIAL,
	PROTO_DEV_USBDEV,
	PROTO_DEV_LIBUSB,
	PROTO_DEV_PROXY,
	PROTO_DEV_STM32
} proto_iodev_devices_t;

typedef struct proto_IfaceIODevice * proto_Device_t;

/// Instance de base de notre interface Device
/// Chaque implémentation d'un IO Device devrait fournir une instance de cette interface.
///
/// Les données complémentaires spécifiques au device sont "attachées" au moyen du champ "user".
/// Exemple de données complémentaires : le FileDescriptor pour le device GNU/Linux
/// Tous les pointeurs de fonction doivent être non-nuls :
/// le protocole appelant considère que c'est une erreur sinon.
typedef struct proto_IfaceIODevice {

    /// \brief Initialisation du device
    /// Ouverture des ressources systèmes.
    /// \param _this l'instance du device, on fera ainsi : monDevice->open(monDevice, "...")
    /// \param szPath chemin système d'accès à la communication ("/dev/ptmx", "/COM1", etc)
    /// \return 0 = OK, sinon erreur
    int  (*open)(proto_Device_t _this, const char * szPath);

    /// \brief Clos la liaison
    /// La mémoire n'est pas désallouée, on peut refaire un open()
    int (*close)(proto_Device_t _this);

    /// \brief Détruit et désalloue le device, le device ne doit plus être utilisé après cela.
    void (*destroy)(proto_Device_t _this);

    /// Lit N caractères. La fonction peut être bloquante
    /// temporairement (timeout) pour certains Device (exemple : GNU/Linux).
    /// \param _this Instance de l'iodevice
    /// \param buf les octets lus seront écrits ici
    /// \param len nombre max d'octets à lire
    /// \return >=0 nombre d'octets lus qui a été lu dans le délai imparti, < 0 erreur
	/// \note : cette fonction ne doit pas générer une erreur si des données ont été lues, 
	///			il s'agit d'un délai imparti pour recevoir des données
    int (*read)(proto_Device_t _this, void* buf, uint8_t len, int16_t tout_ms);

    /// Ecrit N caractères.
    /// \param _this pointeur vers l'instance IO Device
    /// \param buffer octets à envoyer
    /// \param size nombre d'octets à envoyer
    /// \return 0 OK, sinon erreur
    int (*write)(proto_Device_t _this, const void * buffer, uint8_t size);

    /// Données complémentaires de l'interface (FileDescriptor pour GNU/Linux par exemple)
    void * user;
} proto_IfaceIODevice_t;


/// Macro pour accelérer & uniformiser l'initialisation d'un custom device
#define DEVIO_INIT(devname, _this)\
{\
    _this->open = devname##_open;\
    _this->close = devname##_close;\
    _this->destroy = devname##_destroy;\
    _this->read = devname##_read;\
    _this->write = devname##_write;\
}

#ifndef UNUSED
#define UNUSED(x) (void)x
#endif

/** Create a device
	\param type device type we want
	\return proto_Device_t device Handle, NULL if could not be created
*/
proto_Device_t LIBCOMM_EXPORT device_create(proto_iodev_devices_t type);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // IODEVICE_H
