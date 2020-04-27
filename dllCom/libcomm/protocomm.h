#ifndef CIO_TSE_PROTOCOMM
#define CIO_TSE_PROTOCOMM

#include <stdint.h>

/// Une trame est définie par :
/// [HEADER 1 octet] [CRC 1 octet] [COMMAND 1 octet] [ARGS ? octets]
/// Le nombre d'octets pour l'argument dépend de la commande
/// Il peut y avoir des octets à 0 avant le Header ou après les arguments :
/// La réception s'occupe de les ignorer

/// Utilisé pour allouer un buffer sur le tas dans lequel recevoir une trame
enum { proto_FRAME_MAXSIZE = 8 };

/// Les différentes possibilités pour [COMMAND]
typedef enum proto_Command {
	proto_SET = 's',    ///< On veut envoyer une donnée à la cible
	proto_GET = 'g',    ///< On veut lire une donnée de la cible
	proto_RESULT = 'r', ///< La cible répond à la demande de lecture
	proto_ERROR = 'e',  ///< La cible a décelé une erreur
} proto_Command;

/// Retourne le nombre d'octets d'arguments lié à cette commande
inline uint8_t proto_getArgsSize(proto_Command command) {
	switch (command) {
	case proto_SET:    return 2; // [REGISTRE] [VALEUR]
	case proto_GET:    return 1; // [REGISTRE]
	case proto_RESULT: return 1; // [VALEUR]
	case proto_ERROR:  return 1; // [NUM ERREUR]
	default:           return 0;
	}
}

/// Encapsule l'état de l'algorithme analysant les trames.
/// La définition est fournie plus bas dans le header pour
/// pouvoir allouer l'état sur le tas. Cependant, il ne faut
/// pas accéder directement aux attributs de proto_State, mais
/// utiliser les fonctions fournies !
/// Pour initialiser proto_State :
///     proto_State etat = {0};
typedef struct proto_State proto_State;

/// Callback appelé quand une trame est complètement reçue
/// @param[inout] userdata L'utilisateur de la bibliothèque choisit ce qu'il y met et comment l'interpréter
/// @param[in] command La commande demandée
/// @param[in] args Les arguments liés à la commande demandée (de taille proto_getArgsSize(command) )
typedef void(*proto_OnReception)(void* userdata, proto_Command command, uint8_t const* args);

/// Indique quel callback sera appelé quand une trame est complètement reçue.
/// @param[inout] state L'état à modifier
/// @param[in] callback Le callback à appeler, NULL si rien à appeler
/// @param[in] userdata Sera passé tel quel à onReception
void proto_setReceiver(proto_State* state, proto_OnReception callback, void* userdata);

/// Interprète des octets reçus. Appelle le callback spécifié avec
/// proto_setReceiver chaque fois qu'une trame est finie de lire.
/// @param[inout] state L'état qui va interpréter les octets.
/// @param[in] blob Les octets reçus à interpréter.
/// @param[in] size Le nombre d'octets reçus
/// @returns 1 si on a fini de lire au moins une trame avec cet appel, 0 sinon
int proto_readBlob(proto_State* state, uint8_t const* blob, uint8_t size);

/// Construit une trame dans un buffer interne et retourne un pointeur
/// vers ce buffer interne. Le buffer est de taille proto_FRAME_MAXSIZE,
/// mais il n'y a que proto_getArgsSize(command) octets constituant la trame,
/// les autres octets étant mis à 0. Attention, la trame change à chaque appel
/// à cette fonction, il faut donc faire une copie (avec memcpy par exemple)
/// si on veut garder cette trame en mémoire !
/// @param[in] command La commande de la trame.
/// @param[in] args    Les arguments de la trame, de taille proto_getArgsSize(command)
/// @returns un pointeur vers le buffer interne
uint8_t const* proto_writeFrame(proto_Command command, uint8_t const* args);


struct proto_State {
	// Reste à définir en fonction des besoins de l'implémentation...
};

#endif
