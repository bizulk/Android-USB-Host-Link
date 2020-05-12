#ifndef CIO_TSE_PROTOCOMM_EMULSLAVE
#define CIO_TSE_PROTOCOMM_EMULSLAVE

/// L'émulation est basée sur du mono-threadé : quand on appelle write() du device,
/// immédiatement cela appelle interpretBlob() sur une machine d'état interne
/// à "l'émulateur" (cf. proto_Data_EmulSlave_t ). Les octets des trames renvoyées sont
/// stockées dans une queue interne (priv_delayedBytes). Appeler read() du device
/// fait "popper" les octets de cette queue. (PS : la queue est First In, First Out).
/// On peut accéder directement aux registres internes de l'émulateur avec
///     proto_Data_EmulSlave_t devicedata;
///     proto_initData_EmulSlave(&devicedata);
///     ...
///     printf("%d\n", (int)devicedata.registers[3]); // affiche la valeur dans le registre 3
/// Ici, l'émulateur est volontairement limité à 20 registres (de 0 à 19), ainsi
/// l'émulateur émet des INVALID_REGISTER si on tente d'accéder au-delà.
/// Cela permet de tester la détection et réception d'erreur même dans un
/// contexte "parfait" (pas de bruit dans les câbles, pas de délai...).

#include "protocomm.h"
#include "protocomm-details.h" // besoin d'inclure les détails pour définir proto_Data_EmulSlave::priv_state

#ifdef __cplusplus
extern "C" {
#endif

/// Structure de données utiles pour l'émulation de l'esclave.
/// Les attributs préfixés priv_ ne doivent pas être accédés
/// (violation de l'encapsulation).
typedef struct proto_Data_EmulSlave_t {
	uint8_t registers[20]; ///< Les registres qui seront récupérés et modifiés
	proto_State_t priv_state;
	uint8_t priv_delayedBytes[8 * proto_FRAME_MAXSIZE];
	uint8_t priv_nbDelayedBytes;
} proto_Data_EmulSlave_t;

/// Pour initaliser les données du device d'émulation de l'esclave
void proto_initData_EmulSlave(proto_Data_EmulSlave_t* iodata);

/// Retourne le Device (= pointeur vers l'interface read/write). Le iodata à passer
/// dans les fonctions est un pointeur vers un proto_Data_EmulSlave.
proto_Device_t proto_getDevice_EmulSlave(void);

#ifdef __cplusplus
} // extern "C"
#endif

#endif
