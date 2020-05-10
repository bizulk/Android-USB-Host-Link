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


void LIBCOMM_EXPORT proto_slave_init();
void LIBCOMM_EXPORT proto_slave_main(void);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // PROTOCOMM_SLAVE_H
