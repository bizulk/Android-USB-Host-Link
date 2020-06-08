#ifndef PROTOCOMM_SLAVE_H
#define PROTOCOMM_SLAVE_H

/// Définit une API haut-niveau **côté slave** pour communiquer en utilisant ce protocole.
/// L'objectif est de :
///     \arg finaliser l'implementation de la spec protocole : Echanges à réaliser pour répondre aux requêtes du master et les traiter
///     \arg fournir un exemple d'implémentation
///
/// Pour tout autre implémentation slave utiliser protocomm_ll.h
/// Cette implémentation montr
#ifdef __cplusplus
extern "C" {
#endif

#include "protocomm_ll.h"
#include "libcomm_global.h"

///
/// \brief proto_slave_create Creation d'une instance d'esclave
/// \param iodevice
/// \param callback
/// \param userdata
///
///
LIBCOMM_EXPORT proto_hdle_t *  proto_slave_create(proto_Device_t iodevice, proto_OnReception_t callback, void* userdata);


///
/// \brief proto_destroy Destruction de notre instance
/// \param _this Instance
///
LIBCOMM_EXPORT void proto_slave_destroy(proto_hdle_t * _this);

///
/// \brief proto_open Appel l'ouverture du device
/// \return 0 OK, sinon erreur
///
LIBCOMM_EXPORT int proto_slave_open(proto_hdle_t * this, const char * szPath);

///
/// \brief proto_close Appelle la fermeture du device (open possible après)
/// \return 0 OK
///
LIBCOMM_EXPORT int proto_slave_close(proto_hdle_t * this);

///
/// \brief proto_slave_main Iteration de traitement d'un esclave.
/// \return résultat du traitement : 0 pas de requete traitée, 1 une requete traitée, -1 erreur IO
/// Fonction à appeler dans une boucle pour un traitement continu.
///
LIBCOMM_EXPORT int proto_slave_main(proto_hdle_t * this);

enum proto_slave_flags
{
    PROTOSLAVE_FLAG_MASTER_BADCRC=(1), /// si positionné, considère que la trame envoyée par le master a un mauvais CRC
    PROTOSLAVE_FLAG_SLAVE_BADCRC=(1<<1), /// si positionné, le slave repondra avec un CRC mauvais
};

/// Pour générer des cas de test
LIBCOMM_EXPORT void devemulslave_setFlags(proto_Device_t this, uint8_t FLAGS);
/// Pour générer des cas de test
LIBCOMM_EXPORT void devemulslave_getFlags(proto_Device_t this, uint8_t * FLAGS);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // PROTOCOMM_SLAVE_H
