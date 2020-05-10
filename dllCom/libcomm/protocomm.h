#ifndef CIO_TSE_PROTOCOMM
#define CIO_TSE_PROTOCOMM

/// Définit une API haut-niveau **côté master** pour communiquer en utilisant ce protocole.
/// L'objectif est de :
///     \arg finaliser l'implementation de la spec protocole : Echange à réaliser pour lire ou écrire le registre
///     \arg correspondre l' API pandavcom de CIO en terme d'interface : Encapsulation de l'interface "Device" ainsi que de notre instance de protocole
///
/// Pour tout autre implémentation master/slave utiliser protocomm-details.h

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>

#include "protocomm_ll.h"
#include "libcomm_global.h"

///
/// \brief proto_cio_open
/// \param szDev chemin à ouvrir (/dev/ttyS0 ou \\.\COMn)
/// \return Instance de notre protocole (this), ou NULL s'il y a eu un soucis d'implémentation.
///
proto_hdle_t *  LIBCOMM_EXPORT proto_cio_open(const char * szDev, uint16_t timeout_ms);

///
/// \brief proto_cio_close fermeture du projet et libération des ressources
/// \param this
///
void LIBCOMM_EXPORT proto_cio_close(proto_hdle_t * this);

/// Demande à l'esclave une valeur. PS : Le timeout est approximatif
/// car basé sur la fonction clock() de <time.h>. Or cette fonction n'est
/// pas précise (cf. https://en.cppreference.com/w/c/chrono/clock ).
/// @param[in] register_ le registre de l'esclave qu'on veut lire
/// @param[out] value    là où sera écrit la valeur en cas de succès.
/// @pram[in] timeout_ms nombre de millisecondes à attendre au minimum
/// @param[in] iodevice  pointeur vers l'interface du IO Device
/// @param[inout] iodata pointeur vers les données nécessaires pour le IO Device
/// @returns La réponse de l'esclave
proto_Status_t LIBCOMM_EXPORT proto_cio_master_get(proto_hdle_t * this, uint8_t register_, uint8_t* value);

/// Demande à l'esclave d'enregistrer une valeur.
/// @param[in] register_ le registre de l'esclave qu'on veut écrire
/// @param[out] value    la valeur à écrire
/// @pram[in] timeout_ms nombre de millisecondes à attendre au minimum
/// @param[in] iodevice  pointeur vers l'interface du IO Device
/// @param[inout] iodata pointeur vers les données nécessaires pour le IO Device
/// @returns La réponse de l'esclave (la valeur a été écrite ssi proto_NO_ERROR)
proto_Status_t LIBCOMM_EXPORT proto_cio_master_set(proto_hdle_t * this, uint8_t register_, uint8_t value);


#ifdef __cplusplus
} // extern "C"
#endif

#endif
