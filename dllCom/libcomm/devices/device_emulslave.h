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

///
/// \brief devemulslave_getRegisters Cette fonction permet d'avoir accès aux registres du slave émulé
/// \return liste des registres
///
/// Cette fonction existe pour la mise en place des tests
///
uint8_t * LIBCOMM_EXPORT devemulslave_getRegisters(proto_Device_t _this);

enum emulslave_flags
{
    EMULSLAVE_FLAG_MASTER_BADCRC=(1), /// si positionné, considère que la trame envoyée par le master a un mauvais CRC
    EMULSLAVE_FLAG_SLAVE_BADCRC=(1<<1), /// si positionné, le slave repondra avec un CRC mauvais
};

/// Pour générer des cas de test
void LIBCOMM_EXPORT devemulslave_setFlags(proto_Device_t _this, uint8_t FLAGS);
/// Pour générer des cas de test
void LIBCOMM_EXPORT devemulslave_getFlags(proto_Device_t _this, uint8_t * FLAGS);

#ifdef __cplusplus
} // extern "C"
#endif

#endif
