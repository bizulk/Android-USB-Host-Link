#ifndef CIO_TSE_PROTOCOMM
#define CIO_TSE_PROTOCOMM
/// SLI ajouter une entete de fichier, avec une note d'intégration pour l'utilisation.

#include <stdint.h>
#include <assert.h>
#include <stddef.h>

/// Une trame est définie par :
/// [HEADER 1 octet] [CRC 1 octet] [COMMAND 1 octet] [ARGS ? octets]
/// Le nombre d'octets pour l'argument dépend de la commande.
/// Le CRC est calculé sur [COMMAND] [ARGS]
/// Tous les octets avant le Header sont ignorés.

enum {
    proto_MAX_ARGS = 2 ///< Maximum d'arguments, toute commande confondue
};

typedef struct proto_Frame {
    uint8_t header;
    uint8_t crc;
    uint8_t command;
    uint8_t args[proto_MAX_ARGS];
} proto_Frame;

enum { 
    proto_HEADER = 0x12,      ///< La valeur du Header
    proto_COMMAND_OFFSET =    ///< La position de l'octet de commande
        offsetof(proto_Frame, command), 
    proto_ARGS_OFFSET =       ///< La position du premier argument dans la trame
        offsetof(proto_Frame, args),
    proto_FRAME_MAXSIZE =     ///< Utilisé pour allouer un buffer dans lequel recevoir une trame
        sizeof(proto_Frame),
};

/// Les différentes possibilités pour [COMMAND]
typedef enum proto_Command {
    // Commandes du MASTER
        proto_SET,     ///< On veut envoyer une donnée à la cible   : args = [REGISTRE] [VALEUR]
        proto_GET,     ///< On veut lire une donnée de la cible     : args = [REGISTRE]
	// Commandes du SLAVE
        proto_REPLY,   ///< La cible répond à la demande de lecture : args = [VALEUR]
        proto_ERROR,   ///< La cible a décelé une erreur            : args = [NUM ERREUR]
    // Notifications : recevables par le callback mais non transmis physiquement
        proto_BAD_CRC, ///< la trame reçue a un CRC invalide        : args = [CRC_RECU] [CRC_CALCULE]
    // Nombre de commandes possibles
        proto_LAST,    ///< le nombre de commandes disponibles
} proto_Command;

/// Retourne le nombre d'octets d'arguments lié à cette commande
uint8_t proto_getArgsSize(proto_Command command);

/// Encapsule l'état de l'algorithme analysant les trames.
/// La définition est fournie plus bas dans le header pour
/// pouvoir allouer l'état sur la pile. Cependant, il ne faut
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
/// @returns le nombre de trames finies de lire
int proto_readBlob(proto_State* state, uint8_t const* blob, uint8_t size);

/// Construit une trame et l'écrit dans le stockage donné en argument.
/// Les octets d'arguments non utilisés sont mis à zéro.
/// @param[out] frame  Là où sera écrite la trame
/// @param[in] command La commande de la trame.
/// @param[in] args    Les arguments de la trame, de taille proto_getArgsSize(command)
void proto_makeFrame(proto_Frame* frame, proto_Command command, uint8_t const* args);


/// Chaque implémentation d'un IO Device devrait fournir une instance de
/// cette interface afin que le code utilisateur soit découplé du device.
/// Tous les pointeurs de fonction doivent être non-nuls (pour éviter de
/// devoir tout le temps vérifier si ce n'est pas NULL).
/// On peut utiliser proto_openNothing, proto_readNothing,
/// proto_writeNothing et proto_closeNothing si on ne veut pas implémenter
/// certaines de ces fonctions, sans laisser pour autant les pointeurs nuls.
typedef struct proto_IfaceIODevice {
    /// Ouvre un canal de communication.
    /// @param[in] channel Descripteur de la communication ("COM1", "/dev/ttyS0", "/dev/ptmx", etc)
    /// @returns pointeur utilisé en interne pour identifier cette communication
    void* (*open)(char const* channel);
    /// Lit N caractères (possiblement 0), en attendant un certain temps (possiblement 0).
    /// 'timeout_ms' est une indication : on peut attendre plus (par exemple,
    /// l'implémentation GNU/Linux a une précision de 100 ms), et on peut
    /// attendre moins (par exemple, proto_readNothing retourne immédiatement).
    /// @param[in] iodata pointeur qui a été retourné par open()
    /// @param[out] buffer les octets lus seront écrits ici
    /// @param[in] timeout_ms combien de millisecondes attendre avant de retourner
    /// @returns le nombre d'octets lus
    uint8_t (*read)(void* iodata, uint8_t* buffer, uint16_t timeout_ms);
    
    /// Ecrit N caractères.
    /// @param[in] iodata pointeur qui a été retourné par open()
    /// @param[in] buffer octets à envoyer
    /// @param[în] size nombre d'octets à envoyer
    void (*write)(void* iodata, uint8_t const* buffer, uint8_t size);
    
    /// Ferme le canal de communication.
    /// @param[in] iodata pointeur qui a été retourné par open()
    void (*close)(void* iodata);
} proto_IfaceIODevice;

inline void* proto_openNothing(char const* channel) { return NULL; }
inline uint8_t proto_readNothing(void* iodata, uint8_t* buffer, uint16_t timeout_ms) { return 0; }
inline void proto_writeNothing(void* iodata, uint8_t const* buffer, uint8_t size) {}
inline void proot_closeNothing(void* iodata) {}



/// Cette définition n'est fournie que dans le but d'allouer proto_State
/// dans la pile : il ne faut pas accéder aux attributs directement !
/// (violation d'encapsulation)
struct proto_State {
    proto_OnReception priv_callback;
    void* priv_userdata;
    proto_Frame priv_frame;
    uint8_t priv_nbBytes;
};


#endif
