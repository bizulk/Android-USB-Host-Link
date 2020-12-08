#ifndef DEVICE_SERIAL_H
#define DEVICE_SERIAL_H

#ifdef __cplusplus
extern "C" {
#endif

/// Ce fichier implémente un proto device de type port série sous linux
/// L'implémentation permettrait l'ouverture du plusieurs port en parallèle

#include "proto_iodevice.h"
#include "libcomm_global.h"
///
/// \brief Retourne une instance d'interface pour notre interface
/// \param szPath
/// \return Instance de notre device
///
proto_Device_t LIBCOMM_EXPORT devserial_create(void);

/// \brief Si on a déjà un file descriptor (à tout hasard, obtenu avec l'API Androïd), on ouvre avec lui
/// \param _this proto_Device_t qui utilisera ce file descriptor
/// \return 0 si ok, -1 si pas possible de l'utiliser (au quel cas _this n'est pas modifié)
///
int LIBCOMM_EXPORT devserial_setFD(proto_Device_t _this, int fd);

/// \brief Retourne le fileDescriptor, utile pour générer un pt slave correspondant à ptmx
/// \return le file descriptor > 0 s'il existe, ou une valeur < 0 si non-applicable (ex: sous Windows)
///
int LIBCOMM_EXPORT devserial_getFD(proto_Device_t _this);


#ifdef __cplusplus
} // extern "C"
#endif

#endif // DEVICE_SERIAL_H
