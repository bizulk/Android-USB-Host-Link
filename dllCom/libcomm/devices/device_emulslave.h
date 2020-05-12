#ifndef CIO_TSE_PROTOCOMM_EMULSLAVE
#define CIO_TSE_PROTOCOMM_EMULSLAVE

/// Ce device permet du test dans un seul contexte: L'instance du slave est traitée dans le device,
/// Et sa machine a état s'exécute sur les requêtes "master".
/// Cela permet de tester la détection et réception d'erreur

#ifdef __cplusplus
extern "C" {
#endif

#include "proto_iodevice.h"
#include "protocomm_ll.h"
#include "libcomm_global.h"

/// Nombre de registre pour l'émulation
#define EMULSLAVE_NB_REGS 20

///
/// \brief Retourne une instance d'interface pour notre interface
/// \param szPath
/// \return Instance de notre device
///
proto_Device_t LIBCOMM_EXPORT devemulslave_create(void);


#ifdef __cplusplus
} // extern "C"
#endif

#endif
