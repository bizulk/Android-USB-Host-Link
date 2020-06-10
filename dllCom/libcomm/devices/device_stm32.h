#ifndef DEVICE_STM32_H
#define DEVICE_STM32_H

/// Ce fichier implémente un proto device pour la carte stm32
/// Le device "lit" dans une FIFO, qui est alimentée avec les appels devstm32_pushBytes().

#include "proto_iodevice.h"
#include "libcomm_global.h"


/// \brief Retourne une instance d'interface pour notre interface
/// \param szPath
/// \return Instance de notre device
proto_Device_t  devstm32_create(void);


/// \brief Ajouter des octets qui seront lus dans le read() du device
void devstm32_pushBytes(proto_Device_t _this, uint8_t* bytes, uint8_t nbBytes);


#endif // DEVICE_STM32_H
