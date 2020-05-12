#ifndef IODEVICE_H
#define IODEVICE_H

/// Implementation d'un IO Device pour découpler la couche protocole de la liaison
#include <stdint.h>
#include <stddef.h>

typedef struct proto_IfaceIODevice * proto_Device_t;

/// Instance de base de notre interface Device
/// Chaque implémentation d'un IO Device devrait fournir une instance de cette interface.
///
/// Les données complémentaires spécifiques au device sont "attachées" au moyen du champ "user"
/// Tous les pointeurs de fonction doivent être non-nuls : le protocole appelant considère que c'est une erreur sinon.
///
typedef struct proto_IfaceIODevice {
    ///
    /// @brief Initialisation du device
    /// Ouverture des ressources système
    ///
    /// @param[in] path d'accès au device (path système)
    /// @return retour de fonction 0=OK, sinon erreur
    int  (*open)(proto_Device_t this, const char * szPath);

    /// @brief clos la liaison
    /// Un open est toujours possible
    ///
    int (*close)(proto_Device_t this);

    /// Détruit les données de l'interface (par exemple, sous GNU/Linux
    /// cela fera un close() sur le fileDescriptor).
    void (*destroy)(proto_Device_t this);

    /// Lit N caractères. La fonction peut être bloquante
    /// temporairement (timeout) pour certains Device (exemple : GNU/Linux).
    /// @param[in] this Instance de l'iodevice
    /// @param[out] buf les octets lus seront écrits ici
    /// @param[in] len taille à lire
    /// @returns >=0 ci qui a put être lu dans le délai imparti, <0 erreur
    int (*read)(proto_Device_t this, void* buf, uint8_t len, int16_t tout_ms);

    /// Ecrit N caractères.
    /// @param[in] this pointeur vers l'instance IO Device
    /// @param[in] buffer octets à envoyer
    /// @param[in] size nombre d'octets à envoyer
    /// @returns 0 OK, sinon erreur
    int (*write)(proto_Device_t this, const void * buffer, uint8_t size);

    /// Définit les données complémentaires de l'interface
    void * user;
} proto_IfaceIODevice_t;


/// Macro pour accelérer & uniformiser l'initialisation d'un custom device
#define DEVIO_INIT(devname, this)\
{\
    this->open = devname##_open;\
    this->close = devname##_close;\
    this->destroy = devname##_destroy;\
    this->read = devname##_read;\
    this->write = devname##_write;\
}

#ifndef UNUSED
#define UNUSED(x) (void)x
#endif

#endif // IODEVICE_H
