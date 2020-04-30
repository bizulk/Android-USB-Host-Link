#ifndef CIO_TSE_PROTOCOMM
#define CIO_TSE_PROTOCOMM

/// Ce header définit les méthodes d'un Device, ainsi que l'API
/// haut-niveau côté master pour communiquer en utilisant ce protocole.
/// Pour faire des manipulations plus bas-niveau (nécessaires pour implémenter
/// un esclave par exemple), inclure protocomm-details.h
#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif


/// Chaque implémentation d'un IO Device devrait fournir une instance de
/// cette interface afin que le code utilisateur soit découplé du device.
/// L'initialisation n'est pas prise en compte car on ne sait pas comment
/// allouer ni initialiser les données nécessaires pour le device. (exemple :
/// GNU/Linux a juste besoin d'un "int", mais EmulSlave a besoin d'une
/// structure plus grande ; GNU/Linux a besoin d'un char const* pour le
/// chemin du terminal, mais EmulSlave n'a besoin de rien). 
/// Tous les pointeurs de fonction doivent être non-nuls (pour éviter de
/// devoir tout le temps vérifier si ce n'est pas NULL).
typedef struct proto_IfaceIODevice_t {
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
    
    /// Détruit les données de l'interface (par exemple, sous GNU/Linux
    /// cela fera un close() sur le fileDescriptor).
    void (*destroy)(void* iodata);
} proto_IfaceIODevice_t;
typedef proto_IfaceIODevice_t const* proto_Device_t;

/// Appelle `devide->destroy(iodata)`, ni plus ni moins.
void proto_closeDevice(proto_Device_t device, void* iodata);

/// Erreurs possibles
typedef enum proto_Status_t {
    proto_NO_ERROR,
    proto_INVALID_CRC,       ///< la trame reçue n'a pas un CRC cohérent
    proto_INVALID_REGISTER,  ///< la cible ne possède pas le registre demandé
    proto_INVALID_VALUE,     ///< la cible ne peut pas mettre cette valeur dans le registre
    proto_TIMEOUT,           ///< on a pas reçu de trame complète pendant le temps indiqué
} proto_Status_t;


/// Demande à l'esclave une valeur. PS : Le timeout est approximatif
/// car basé sur la fonction clock() de <time.h>. Or cette fonction n'est
/// pas précise (cf. https://en.cppreference.com/w/c/chrono/clock ).
/// @param[in] register_ le registre de l'esclave qu'on veut lire
/// @param[out] value    là où sera écrit la valeur en cas de succès.
/// @pram[in] timeout_ms nombre de millisecondes à attendre au minimum
/// @param[in] iodevice  pointeur vers l'interface du IO Device
/// @param[inout] iodata pointeur vers les données nécessaires pour le IO Device
/// @returns La réponse de l'esclave
proto_Status_t proto_cio_master_get(uint8_t register_, uint8_t* value, 
                    uint16_t timeout_ms, proto_Device_t device, void* iodata);

/// Demande à l'esclave d'enregistrer une valeur.
/// @param[in] register_ le registre de l'esclave qu'on veut écrire
/// @param[out] value    la valeur à écrire
/// @pram[in] timeout_ms nombre de millisecondes à attendre au minimum
/// @param[in] iodevice  pointeur vers l'interface du IO Device
/// @param[inout] iodata pointeur vers les données nécessaires pour le IO Device
/// @returns La réponse de l'esclave (la valeur a été écrite ssi proto_NO_ERROR)
proto_Status_t proto_cio_master_set(uint8_t register_, uint8_t value,
                    uint16_t timeout_ms, proto_Device_t device, void* iodata);


#ifdef __cplusplus
} // extern "C"
#endif

#endif
