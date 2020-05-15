#ifndef DEVICE_SERIAL_H
#define DEVICE_SERIAL_H

#ifdef __cplusplus
extern "C" {
#endif

/// Ce fichier implémente un proto device de type port série sous windows
/// TODO serait contaténable avec Linux ? je sais plus...
/// L'implémentation permettrait l'ouverture du plusieurs port en parallèle
///
///

#include "proto_iodevice.h"
#include "libcomm_global.h"
///
/// \brief Retourne une instance d'interface pour notre interface
/// \param szPath
/// \return Instance de notre device
///
proto_Device_t LIBCOMM_EXPORT devserial_create(void);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // DEVICE_SERIAL_H
