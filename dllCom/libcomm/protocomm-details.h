#ifndef CIO_TSE_PROTOCOMM_DETAILS
#define CIO_TSE_PROTOCOMM_DETAILS

/**
 * Ce header inclut les "détails" du protocole, à savoir : la composition de la trame,
 * et des fonctions bas-niveau pour manipuler et émettre ces trames.
 * Cette API est notamment utilisé pour implémenter un esclave.
 * (on peut aussi implémenter un MASTER avec, mais il y a une API plus
 * haut niveau dans protocomm.h)
 */
#include <stdint.h>
#include <stddef.h>

#include "protocomm.h"

#ifdef __cplusplus
extern "C" {
#endif

#define proto_MAX_ARGS 2

/// Structure qui montre l'organisation d'une trame.
/// Le nombre d'arguments est variable (dépend de la commande).
/// Les octets à envoyer sont donc les N premiers octets de la structure,
/// où N = proto_ARGS_OFFSET + proto_getArgsSize(command).
/// Cependant, interpretBlob() ignore les octets d'après, tant qu'on a
/// pas reçu "StartOfFrame", or les octets d'arguments non-utilisés sont mis
/// à 0 dans makeFrame, donc on peut tout de même envoyer tous les octets
/// de la structure.
typedef struct proto_Frame_t {
    uint8_t startOfFrame;
    uint8_t crc8;
    uint8_t command; /// ce sont les valeurs de l'enum proto_Command_t
    uint8_t args[proto_MAX_ARGS];
} proto_Frame_t; 

/// Même si la trame est bien définie, cela permet d'enlever les
/// constantes magiques du code.
enum { 
    proto_START_OF_FRAME = 0x12, ///< La valeur du Start of Frame
    proto_COMMAND_OFFSET =       ///< La position de l'octet de commande
        offsetof(proto_Frame_t, command), 
    proto_ARGS_OFFSET =          ///< La position du premier argument dans la trame
        offsetof(proto_Frame_t, args),
    proto_FRAME_MAXSIZE =        ///< Utilisé pour allouer un buffer dans lequel recevoir une trame
        sizeof(proto_Frame_t),
};


/// Les commandes qui sont recevables dans le callback.
/// Il y a deux types d'erreurs :
/// - distantes : la cible n'a pas réussi à traiter notre demande :
///               (registre invalide, mauvais CRC...) 
/// - locales : on a pas réussi à comprendre la trame reçue :
///               (les CRC reçu et calculé ne correspondent pas...)
/// Les erreurs distantes font partie de la commande proto_STATUS,
/// dont l'argument 0 correspond à une valeur de proto_Status_t.
/// Les erreurs locales sont traduites par des "notifications"
/// ( proto_NOTIF_* ). Elles ne sont pas envoyées sur la liaison :
/// Elles existent seulement dans le callback ! (par exemple, un esclave
/// ayant reçu une NOTIF_BAD_CRC renverra une trame [ STATUS, INVALID_CRC ].
typedef enum proto_Command_t {
    // Commandes du MASTER
        proto_SET,     ///< On veut envoyer une donnée à la cible   : args = [REGISTRE] [VALEUR]
        proto_GET,     ///< On veut lire une donnée de la cible     : args = [REGISTRE]
    // Commandes du SLAVE
        proto_REPLY,   ///< La cible répond à la demande de lecture : args = [VALEUR]
        proto_STATUS,  ///< La cible répond OK ou une erreur        : args = [proto_Status_t]
    // Notifications : recevables par le callback mais non transmis physiquement
        proto_NOTIF_BAD_CRC, ///< la trame reçue a un CRC invalide  : args = [CRC_RECU] [CRC_CALCULE]
    // Nombre de commandes possibles
        proto_LAST,    ///< le nombre de commandes disponibles
} proto_Command_t;

// définition de proto_Status_t dans "protocomm.h"


/// Retourne le nombre d'octets d'arguments lié à cette commande
uint8_t proto_getArgsSize(proto_Command_t command);


/// Encapsule l'état de l'algorithme analysant les trames.
/// La définition est fournie plus bas dans le header pour
/// pouvoir allouer l'état sur la pile. Cependant, il ne faut
/// pas accéder directement aux attributs de proto_State, mais
/// utiliser les fonctions fournies !
/// Pour initialiser proto_State :
///     proto_State etat = {0};
typedef struct proto_State_t proto_State_t;

/// Callback appelé quand une trame est complètement reçue
/// @param[inout] userdata L'utilisateur de la bibliothèque choisit ce qu'il y met et comment l'interpréter
/// @param[in] command La commande demandée
/// @param[in] args Les arguments liés à la commande demandée (de taille proto_getArgsSize(command) )
typedef void(*proto_OnReception_t)(void* userdata, proto_Command_t command, uint8_t const* args);

/// Indique quel callback sera appelé quand une trame est complètement reçue.
/// @param[inout] state L'état à modifier
/// @param[in] callback Le callback à appeler, NULL si rien à appeler
/// @param[in] userdata Sera passé tel quel à onReception
void proto_setReceiver(proto_State_t* self, proto_OnReception_t callback, void* userdata);


/// Interprète des octets reçus. Appelle le callback spécifié avec
/// proto_setReceiver si une trame est finie de lire. S'arrête à la première
/// trame lue. La fonction est publique car on peut vouloir ne pas passer par
/// un device (par exemple, sur la cible, les octets sont reçus via un
/// callback et non par un appel à une méthode "read()", donc la cible ne
/// peut pas utiliser un device et doit directement passer les octets
/// reçus à proto_interpretBlob).
/// @param[inout] state L'état qui va interpréter les octets.
/// @param[in] blob Les octets reçus à interpréter.
/// @param[in] size Le nombre d'octets reçus
/// @returns le nombre de trames lues, y compris celles ayant des erreurs.
int proto_interpretBlob(proto_State_t* state, uint8_t const* blob, uint8_t size);


/// Construit une trame et l'écrit dans le stockage donné en argument.
/// Les octets d'arguments non utilisés sont mis à zéro.
/// @param[out] frame  Là où sera écrite la trame
/// @param[in] command La commande de la trame.
/// @param[in] args    Les arguments de la trame, de taille proto_getArgsSize(command)
/// @returns le nombre d'octets composant la trame, <= à proto_FRAME_MAXSIZE
uint8_t proto_makeFrame(proto_Frame_t* frame, proto_Command_t command, uint8_t const* args);


/// Fonction d'aide qui fait le lien entre le device et l'état.
/// @param[in] state L'état qui va interpréter les octets lus.
/// @param[in] iodevice pointeur vers l'interface du IO Device
/// @param[inout] iodata pointeur vers les données nécessaires pour le IO Device
/// @returns le nombre de trames finies de lire
int proto_readBlob(proto_State_t* state,
                   proto_Device_t iodevice, void* iodata);
                   
                   
/// Fonction d'aide pour envoyer directement une trame par le device.
/// @param[in] command La commande de la trame.
/// @param[in] args Les arguments de la trame, de taille proto_getArgsSize(command)
/// @param[in] iodevice pointeur vers l'interface du IO Device
/// @param[inout] iodata pointeur vers les données nécessaires pour le IO Device
void proto_writeFrame(proto_Command_t command, uint8_t const* args,
                      proto_Device_t iodevice, void* iodata);




/// Cette définition n'est fournie que dans le but d'allouer proto_State
/// dans la pile : il ne faut pas accéder aux attributs directement !
/// (violation d'encapsulation)
struct proto_State_t {
    proto_OnReception_t priv_callback;
    void* priv_userdata;
    proto_Frame_t priv_frame;
    uint8_t priv_nbBytes;
};

#ifdef __cplusplus
} // extern "C"
#endif

#endif
