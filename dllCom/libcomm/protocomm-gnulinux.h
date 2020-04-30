#ifndef CIO_TSE_PROTOCOMM_GNULINUX
#define CIO_TSE_PROTOCOMM_GNULINUX

#include "protocomm.h"

#ifdef __cplusplus
extern "C" {
#endif

/// Structure de données utiles pour le device sous GNU/Linux
/// fileDescriptor est laissé public car l'utilisateur peut vouloir
/// modifier des choses qui ne dépendent pas de la bibliothèque.
/// Par exemple pour /dev/ptmx, il faudra faire grantpt() et unlockpt(),
/// mais ce n'est pas le cas pour tous les terminaux.
typedef struct proto_Data_GnuLinux_t {
	int fileDescriptor; ///< En lecture seule ! (pour ouvrir le ptmx esclave par exemple)
} proto_Data_GnuLinux_t;

/// Pour initialiser les données du device (ici, ouvrir un fileDescriptor)
/// @param[out] iodata Les données du device à écrire
/// @param[in]  path   Le chemin du terminal : "/dev/ptmx", "/dev/ttyS0"...
/// @param[in] timeout_ds Temps à attendre avant que read() ne retourne, en décisecondes (timeout = 10 <=> 1 seconde) (peut être 0)
/// @returns 0 si tout s'est bien passé, un nombre négatif sinon
int proto_initData_GnuLinux(proto_Data_GnuLinux_t* iodata, char const* path, uint8_t timeout_ds);

/// Retourne le Device (= pointeur vers l'interface read/write). Le iodata à passer
/// dans les fonctions est un pointeur vers un proto_Data_GnuLinux.
proto_Device_t proto_getDevice_GnuLinux();

#ifdef __cplusplus
} // extern "C"
#endif

#endif
