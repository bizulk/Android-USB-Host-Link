#ifndef DEVICE_SERIAL_H
#define DEVICE_SERIAL_H

/// Ce fichier implémente un proto device de type port série sous windows
/// TODO serait contaténable avec Linux ? je sais plus...
/// L'implémentation permettrait l'ouverture du plusieurs port en parallèle
///
///

#include "proto_iodevice.h"

///
/// \brief Retourne une instance d'interface pour notre interface
/// \param szPath
/// \return Instance de notre device
///
proto_Device_t devserial_create(void);

///
/// \brief devserial_init : même chose que create, mais avec une instance alloué sur la pile
/// \warning Ne pas appeler "destroy" pour ce cas
/// \param this Instance pre-alloué
///
void devserial_init(proto_Device_t this);

///
/// \brief devserial_destroy Détruit notre instance
/// \param this Instance de notre device
void devserial_destroy(proto_Device_t this);

#endif // DEVICE_SERIAL_H
