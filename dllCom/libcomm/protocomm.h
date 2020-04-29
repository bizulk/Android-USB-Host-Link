#ifndef CIO_TSE_PROTOCOMM
#define CIO_TSE_PROTOCOMM
/// SLI ajouter une entete de fichier, avec une note d'intégration pour l'utilisation.

#if 0
    // Exemple d'utilisation pour un master

    // master_receive sera appelé quand on a reçu une trame
    void master_receive(void* userdata, proto_Command command, uint8_t const* args) {
        switch (command) {
        case proto_REPLY:
            *(uint8_t*)userdata = args[0];
            break;
        case proto_NOTIF_BAD_CRC:
            printf("Mauvais CRC... reçu=%d, calculé=%d\n", args[0], args[1]);
            break;
        case proto_STATUS:
            if (args[0] != proto_NO_ERROR)
                printf("Erreur reçue : %d\n", args[0]);
            break;
        }
    }

    int monMain() {
        // remplacer IMPLEM par l'implémentation choisie (par exemple EmulSlave)
        proto_Data_IMPLEM devicedata;
        proto_initData_IMPLEM(&devicedata);
        proto_Device device = proto_getDevice_IMPLEM();
        
        proto_State etat = {0};
        
        uint8_t args[proto_MAX_ARGS];
        uint8_t numeroRegistre = 5; // registre à modifier, ici on a choisi le registre n°5
        
        // Faire un SET
        args[0] = numeroRegistre;
        args[1] = 234; // pour mettre 234 dans le registre "numeroRegistre"
        proto_writeFrame(proto_SET, args, device, &devicedata);    
        
        // Faire un GET
        // Quand on reçoit proto_REPLY, on met la valeur récupérée dans registreLu
        uint8_t valeurRegistre = 0;
        proto_setReceiver(&state, master_receive, &valeurRegistre);
        // on fait une requête GET
        args[0] = numeroRegistre;
        proto_writeFrame(proto_GET, args, device, &devicedata);
        while (!proto_readBlob(&etat, device, &devicedata)); // tant qu'on a pas réussi à lire une trame, on réessaye
        printf("Valeur du registre %d = %d\n", numeroRegistre, valeurRegistre);
        
    }

    // Fin de l'exemple
#endif

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

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
        proto_STATUS,  ///< La cible répond OK ou une erreur        : args = [proto_Status]
    // Notifications : recevables par le callback mais non transmis physiquement
        proto_NOTIF_BAD_CRC, ///< la trame reçue a un CRC invalide        : args = [CRC_RECU] [CRC_CALCULE]
    // Nombre de commandes possibles
        proto_LAST,    ///< le nombre de commandes disponibles
} proto_Command;

/// Erreurs possibles pour la cible
typedef enum proto_Status {
    proto_NO_ERROR,
    proto_INVALID_CRC,         ///< la cible a eu une erreur de CRC sur la trame qu'elle a reçue
    proto_INVALID_REGISTER,  ///< la cible ne possède pas le registre demandé
    proto_INVALID_VALUE,     ///< la cible ne peut pas mettre cette valeur dans le registre
} proto_Status;

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
int proto_interpretBlob(proto_State* state, uint8_t const* blob, uint8_t size);

/// Construit une trame et l'écrit dans le stockage donné en argument.
/// Les octets d'arguments non utilisés sont mis à zéro.
/// @param[out] frame  Là où sera écrite la trame
/// @param[in] command La commande de la trame.
/// @param[in] args    Les arguments de la trame, de taille proto_getArgsSize(command)
/// @returns le nombre d'octets composant la trame, <= à proto_FRAME_MAXSIZE
uint8_t proto_makeFrame(proto_Frame* frame, proto_Command command, uint8_t const* args);


/// Chaque implémentation d'un IO Device devrait fournir une instance de
/// cette interface afin que le code utilisateur soit découplé du device.
/// L'initialisation et la destruction du Device ne sont pas pris en compte
/// dans l'interface car de toute façon, il faudra forcément modifier l'initialisation
/// en changeant d'implémentation (changement de header, passer de "/dev/ptmx" à "COM1"...).
/// Tous les pointeurs de fonction doivent être non-nuls (pour éviter de
/// devoir tout le temps vérifier si ce n'est pas NULL).
typedef struct proto_IfaceIODevice {
    /// Lit N caractères (possiblement 0). La fonction peut être bloquante
    /// temporairement (timeout) pour certains Device (exemple : GNU/Linux).
    /// @param[in] iodata pointeur vers le type encapsulant le IO Device
    /// @param[out] buffer les octets lus seront écrits ici
    /// @param[in] bufferSize le nombre d'octets maximal à écrire dans le buffer
    /// @returns le nombre d'octets lus
    uint8_t (*read)(void* iodata, uint8_t* buffer, uint8_t bufferSize);
    
    /// Ecrit N caractères.
    /// @param[in] iodata pointeur vers le type encapsulant le IO Device
    /// @param[in] buffer octets à envoyer
    /// @param[în] size nombre d'octets à envoyer
    void (*write)(void* iodata, uint8_t const* buffer, uint8_t size);
} proto_IfaceIODevice;
typedef proto_IfaceIODevice const* proto_Device;

/// Fonction d'aide qui fait le lien entre le device et l'état.
/// @param[in] state L'état qui va interpréter les octets lus.
/// @param[in] iodevice pointeur vers l'interface du IO Device
/// @param[inout] iodata pointeur vers les données nécessaires pour le IO Device
/// @returns le nombre de trames finies de lire
int proto_readBlob(proto_State* state,
                   proto_Device iodevice, void* iodata);

/// Fonction d'aide pour envoyer directement une trame par le device.
/// @param[in] command La commande de la trame.
/// @param[in] args Les arguments de la trame, de taille proto_getArgsSize(command)
/// @param[in] iodevice pointeur vers l'interface du IO Device
/// @param[inout] iodata pointeur vers les données nécessaires pour le IO Device
void proto_writeFrame(proto_Command command, uint8_t const* args,
                      proto_Device iodevice, void* iodata);

/// Cette définition n'est fournie que dans le but d'allouer proto_State
/// dans la pile : il ne faut pas accéder aux attributs directement !
/// (violation d'encapsulation)
struct proto_State {
    proto_OnReception priv_callback;
    void* priv_userdata;
    proto_Frame priv_frame;
    uint8_t priv_nbBytes;
};

#ifdef __cplusplus
} // extern "C"
#endif

#endif
