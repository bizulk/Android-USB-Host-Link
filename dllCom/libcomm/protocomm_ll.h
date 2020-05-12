#ifndef CIO_TSE_PROTOCOMM_DETAILS
#define CIO_TSE_PROTOCOMM_DETAILS

/**
 * API LowLevel du protocole : manipulation bas niveau des trames
 *
 * Cette API est notamment utilisé pour implémenter un esclave.
 * (on peut aussi implémenter un MASTER avec, mais il y a une API plus
 * haut niveau dans protocomm.h)
 */
#include <stdint.h>
#include <stddef.h>

#include "proto_iodevice.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 * API - Format d'échanche
******************************************************************************/

/// Erreurs possibles dans les traitements
typedef enum proto_Status {
    proto_NO_ERROR,
    proto_ERR_SYS,           ///!< Erreur systeme
    proto_ERR_CRC,       ///< la trame reçue n'a pas un CRC cohérent
    proto_INVALID_ARG,  ///< la cible ne possède pas le registre demandé
    proto_TIMEOUT,           ///< on a pas reçu de trame complète pendant le temps indiqué
    proto_ERR_PROTO ///< Erreur de protocole (réponse inattendue)
} proto_Status_t;

#define proto_MAX_ARGS 2


/// Définit les arguments d'une trame
typedef struct proto_frame_arg
{
   uint8_t reg; ///< numéro de registre à lire ou écrire
   uint8_t value; ///< Valeur du registre
} proto_frame_arg_t;

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
        proto_CMD_SET,     ///< On veut envoyer une donnée à la cible   : args = [REGISTRE] [VALEUR]
        proto_CMD_GET,     ///< On veut lire une donnée de la cible     : args = [REGISTRE] (PADDING)
    // Réponse du SLAVE
        proto_CMD_REPLY,   ///< La cible répond à la demande de lecture : args = [VALEUR]
        proto_CMD_ERR_CRC,  ///< la trame reçue a un CRC invalide        : args = [CRC_RECU] [CRC_CALCULE]:
        proto_CMD_ERR_ARG, ///< le traitement utilisateur a refusé le traitement : args = [REGISTRE] (VALEUR) (qui ont été envoyé)
    // Nombre de commandes possibles
        proto_LAST,    ///< le nombre de commandes disponibles
} proto_Command_t;

/// Structure qui montre l'organisation d'une trame.
/// Par simplicité on ne gère que l'échange d'un registre.
/// Toute la trame peut-être envoyée, même si value n'est pas utilisée (requête de lecture du master)
/// Pour éviter les problèmes de padding on explicite les types à utiliser
typedef struct proto_Frame {
    uint8_t startOfFrame; ///< synchronisation de la réception
    uint8_t crc8; ///< CRC8
    uint8_t command; ///< ce sont les valeurs de l'enum proto_Command_t (command pour le master, status pour le slave)
    union {
        proto_frame_arg_t arg; ///< registre
        uint8_t err; ///< Code d'erreur
    };
} proto_Frame_t; 

/// Constantes pour la manipulation de la trame
enum { 
    proto_START_OF_FRAME = 0x12, ///< La valeur du Start of Frame
    proto_COMMAND_OFFSET =       ///< La position de l'octet de commande
        offsetof(proto_Frame_t, command), 
    proto_ARGS_OFFSET =          ///< La position du premier argument dans la trame
        offsetof(proto_Frame_t, arg),
    proto_FRAME_MAXSIZE =        ///< Utilisé pour allouer un buffer dans lequel recevoir une trame
        sizeof(proto_Frame_t),
};

/// Esclave : Callback appelée quand une trame est complètement reçue
/// @param[inout] userdata L'utilisateur de la bibliothèque choisit ce qu'il y met et comment l'interpréter
/// @param[in] command La commande demandée
/// @param[inout] args Les arguments liés à la commande demandée (de taille proto_getArgsSize(command) ),
/// @return résultat du traitement : 0 commande traitée, sinon erreur dans les arguments
typedef int (*proto_OnReception_t)(void* userdata, proto_Command_t command, uint8_t * args);

/// Instance de protocole Master/Slave
/// Cette définition n'est fournie que dans le but d'allouer proto_State
/// dans la pile : il ne faut pas accéder aux attributs directement !
/// (violation d'encapsulation)
typedef struct proto_hdle {
    proto_OnReception_t priv_callback; ///< callabck de reception d'une trame complete
    void* priv_userdata; ///< User data associé à la callback
    proto_Frame_t priv_frame; ///< Instance d'une frame
    uint8_t priv_nbBytes; ///< Curseur de transmission des données
    proto_Device_t priv_iodevice; ///< Device à utiliser
} proto_hdle_t;

/// Statut en résultat du décodage de trame
typedef enum proto_DecodeStatus {
    proto_WAITING,   /// La trame n'est pas finie
    proto_COMPLETED, /// On a lu une trame correctement
    proto_REFUSED    /// On a lu une trame mais il y a eu une erreur (exemple : CRC)
} proto_DecodeStatus_t;

/******************************************************************************
 * PROTO COMMUN SLAVE/MASTER
******************************************************************************/

///
/// \brief proto_create Créer une instance de protocole
/// \param iodevice device à utiliser (peut être NULL si on ne souhaite pas utiliser l'API)
/// \return Instance de protocole, NULL si n'a pu être alloué
///
proto_hdle_t * proto_create(proto_Device_t iodevice);

///
/// \brief proto_init Même chose que create, mais pour une instance fournie à l'init
/// \param this Instance du protocole
/// \param iodevice Instance de l'IO device
///
/// \warning ne pass appeler destroy dans ce cas de figure
///
void proto_init(proto_hdle_t * this, proto_Device_t iodevice);

///
/// \brief proto_destroy detruit toutes les ressources alloué
/// \param this après destruction ce handle est invalide
///
void proto_destroy(proto_hdle_t * this);

///
/// \brief proto_open Appel l'ouverture du device
/// \return 0 OK, sinon erreur
///
int proto_open(proto_hdle_t * this, const char * szPath);

///
/// \brief proto_close Appelle la fermeture du device (open possible après)
/// \return 0 OK
///
int proto_close(proto_hdle_t * this);

#define PROTO_WAIT_FOREVER -1

/// Fonction de réception d'une trame (maitre ou esclave)
/// **Utilise le device** pour une lecture,
/// La fonction tente de lire une trame dans le délai imparti.
/// Le délai est appliqué par le device utilisé.
///
/// Note : pour dépiler la trame reçue il faut appeler proto_decodeFrame (tout appel répété annulera la lecture)
///
/// \arg this instance
/// \arg tout en milliseconde, si <0 attente infinie, si 0 non bloquant, sinon on attend le délai
/// @returns OK pour une trame reçue, TIMEOUT sinon, ERR_SYS si problème sur le read
proto_Status_t proto_readFrame(proto_hdle_t* this, int16_t tout_ms);

/// Ecriture d'une trame
///
/// Maitre : A utiliser pour envoyer une requete, Esclave : pour envoyer la réponse.
/// @param[in] Instance du protocole
/// @param[in] command La commande de la trame.
/// @param[in] args Les arguments de la trame, cf le format de la frame pour l'ordre
/// @return résultat de l'opération (0 réussi, sinon erreur)
int proto_writeFrame(proto_hdle_t* this, proto_Command_t command, const uint8_t * args);

/******************************************************************************
 * PROTO RESERVE SLAVE
******************************************************************************/

/// Retourne le nombre d'octets d'arguments lié à cette commande
uint8_t proto_getArgsSize(proto_Command_t cmd);

/// Indique quel callback Utilisateur sera appelé quand une trame est complètement reçue.
/// @param[inout] state L'état à modifier
/// @param[in] callback Le callback à appeler, NULL si rien à appeler
/// @param[in] userdata Sera passé tel quel à onReception
void proto_setReceiver(proto_hdle_t* this, proto_OnReception_t callback, void* userdata);


/// Empilement et analyse des octets recus
/// \note La fonction est publique pour permettre une simplification à l'intégration : bypasser l'utilisation d'un device,
/// Exemple : sur la carte cible on pourra appeler directement cette fonction dans la callback USB de réception de caractères.
/// note : pour dépiler la trame reçue il faut appeler proto_decodeFrame
/// @param[inout] Instance du protocole.
/// @param[in] buf données d'entrées (peut être > trame)
/// @param[in] len Le nombre d'octets d'entrée.
/// @returns le nombre d'octets consommés [0-len] => trame pleine, -1 : pas de trame validé
int proto_pushToFrame(proto_hdle_t* this, const uint8_t * buf, uint32_t len);

///
/// \brief proto_decodeFrame Décodage de la trame reçue et dépilement
/// \param this Instance de protocole
/// \param[out] cmd commande
/// \param[out] arg données associées
/// \return résultat de l'analyse : WAITING (en attente de trame), COMPLETED (une trame décodée), REFUSED : trame invalide
///
proto_DecodeStatus_t proto_decodeFrame(proto_hdle_t* this, proto_Command_t * cmd, proto_frame_arg_t *arg);

/// Construit une trame d'échange
/// Fonction publique : si l'on utilise le protocole sans IOdevice, sinon utiliser proto_writeFrame
/// Les octets d'arguments non utilisés sont mis à zéro.
/// @param[out] frame  Là où sera écrite la trame
/// @param[in] command La commande de la trame.
/// @param[in] args    Les arguments de la trame, dans l'ordre donné par proto_frame_arg_t ou code d'erreur
/// @returns le nombre d'octets composant la trame, <= à proto_FRAME_MAXSIZE, 0 si un argument invalide
uint8_t proto_makeFrame(proto_Frame_t* frame, proto_Command_t command, const uint8_t * args);

#ifdef __cplusplus
} // extern "C"
#endif

#endif
