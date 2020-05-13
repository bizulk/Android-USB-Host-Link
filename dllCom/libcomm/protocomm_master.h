#ifndef CIO_TSE_PROTOCOMM
#define CIO_TSE_PROTOCOMM

/// Définit une API haut-niveau **côté master** pour communiquer en utilisant ce protocole.
/// L'objectif est de :
///     \arg finaliser l'implementation de la spec protocole : Echange à réaliser pour lire ou écrire le registre
///     \arg correspondre l' API pandavcom de CIO en terme d'interface : Encapsulation de l'interface "Device" ainsi que de notre instance de protocole

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>

#include "protocomm_ll.h"
#include "libcomm_global.h"

/// Définition de l'attente d'une trame
#define PROTO_FRAME_RECV_TOUT_MS 1000

///
/// \brief proto_cio_open Encapsulation du protocole pour avoir une API confirme
/// \param szDev chemin à ouvrir (/dev/ttyS0 ou \\.\COMn)
/// \return Instance de notre protocole (_this), ou NULL s'il y a eu un soucis d'implémentation.
///
/// Encapsule la creation du device
///
proto_hdle_t *  LIBCOMM_EXPORT proto_cio_open(const char * szDev);

///
/// \brief proto_master_create Creation d'une instance pour le master
/// \param iodevice Instance du device
/// \param callback
/// \param userdata
///
proto_hdle_t * LIBCOMM_EXPORT proto_master_create(proto_Device_t iodevice);

///
/// \brief proto_destroy Destruction de notre instance
/// \param _this Instance
///
void LIBCOMM_EXPORT proto_master_destroy(proto_hdle_t * _this);

///
/// \brief proto_open Appel l'ouverture du device
/// \return 0 OK, sinon erreur
///
int LIBCOMM_EXPORT proto_master_open(proto_hdle_t * _this, const char * szPath);

///
/// \brief proto_close Appelle la fermeture du device (open possible après)
/// \return 0 OK
///
int LIBCOMM_EXPORT proto_master_close(proto_hdle_t * _this);


/// Demande à l'esclave une valeur. PS : Le timeout est approximatif
/// car basé sur la fonction clock() de <time.h>. Or cette fonction n'est
/// pas précise (cf. https://en.cppreference.com/w/c/chrono/clock ).
/// @param[in] register_ le registre de l'esclave qu'on veut lire
/// @param[out] value    là où sera écrit la valeur en cas de succès.
/// @pram[in] timeout_ms nombre de millisecondes à attendre au minimum
/// @param[in] iodevice  pointeur vers l'interface du IO Device
/// @param[inout] iodata pointeur vers les données nécessaires pour le IO Device
/// @returns La réponse de l'esclave
proto_Status_t LIBCOMM_EXPORT proto_master_get(proto_hdle_t * _this, uint8_t register_, uint8_t* value);

/// Demande à l'esclave d'enregistrer une valeur.
/// @param[in] register_ le registre de l'esclave qu'on veut écrire
/// @param[out] value    la valeur à écrire
/// @pram[in] timeout_ms nombre de millisecondes à attendre au minimum
/// @param[in] iodevice  pointeur vers l'interface du IO Device
/// @param[inout] iodata pointeur vers les données nécessaires pour le IO Device
/// @returns La réponse de l'esclave (la valeur a été écrite ssi proto_NO_ERROR)
proto_Status_t LIBCOMM_EXPORT proto_master_set(proto_hdle_t * _this, uint8_t register_, uint8_t value);


#ifdef __cplusplus
} // extern "C"
#endif

#endif
