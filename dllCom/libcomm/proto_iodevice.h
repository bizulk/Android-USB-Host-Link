#ifndef IODEVICE_H
#define IODEVICE_H

/// Implementation d'un IO Device pour découpler la couche protocole de la liaison
#include <stdint.h>
#include <stddef.h>


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
    int  (*open)(struct proto_IfaceIODevice* this, const char * szPath);

    /// @brief clos la liaison
    /// Un open est toujours possible
    ///
    int (*close)(struct proto_IfaceIODevice* this);

    /// Détruit les données de l'interface (par exemple, sous GNU/Linux
    /// cela fera un close() sur le fileDescriptor).
    void (*destroy)(struct proto_IfaceIODevice* this);

    /// Lit N caractères. La fonction peut être bloquante
    /// temporairement (timeout) pour certains Device (exemple : GNU/Linux).
    /// @param[in] this Instance de l'iodevice
    /// @param[out] buf les octets lus seront écrits ici
    /// @param[in] len taille à lire
    /// @returns >=0 ci qui a put être lu dans le délai imparti, <0 erreur
    int (*read)(struct proto_IfaceIODevice* this, void* buf, uint8_t len, int16_t tout_ms);

    /// Ecrit N caractères.
    /// @param[in] this pointeur vers l'instance IO Device
    /// @param[in] buffer octets à envoyer
    /// @param[in] size nombre d'octets à envoyer
    /// @returns 0 OK, sinon erreur
    int (*write)(struct proto_IfaceIODevice* this, const void * buffer, uint8_t size);

    /// Définit les données complémentaires de l'interface
    void * user;
} proto_IfaceIODevice_t;


typedef proto_IfaceIODevice_t * proto_Device_t;


#endif // IODEVICE_H
