#ifndef LOG_H
#define LOG_H

/** Auteur : Selso LIBERADO
 * Interface de log pour ajouter des traces d'exécution dans la lib.
 * Pour répondre aussi au besoin de debug filaire sous android on propose une FIFO de messages
 * On peut sélectionner à la compilation un fonctionnement à base de printf/ FIFO
 * Dans le cas d'un printf null pas de fifo utilisé : la trace est directement affichée TODO TODO
 *
 * \warning inutile d'inclure ce code dans le STM32, mais si besoin attention à la taille de la pile,
 * voir revoir ce code pour allouer dans la sdram
*/

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdint.h>

#include "libcomm_global.h"

/// Taille moyenne d'un message
/// \sa log_push
#define LOG_MSG_LEN 100

/// si défini, alors on utilisera une fifo, sinon on utilisera un printf
#define LOG_USE_FIFO

typedef struct log_handle * log_phandle_t;

/** Helper pour envoyer un message de log d'exécution
 * Ajoute l'emplacement 'fonction' et le numéro de ligne au message
*/
#define LOG_PUSH(_this, szMg, format, args...) do\
{\
    snprintf( szMg, LOG_MSG_LEN, "%s:%d -"##format, __FUNCTION__, __LINE__ -2,##args);\
    log_push(_this, szMg);\
 } while(0);

/** Création de notre log
    @param nbMsg nombre maximal de message
    @return retour d'initialisation, NULL si erreur
*/
LIBCOMM_EXPORT log_phandle_t log_init(uint32_t nbMsg);

/**
 * @brief log_destroy libération des ressources
 * @return 0 OK, -1 erreur d'argument
 *
 * \warning comportement indéterminé si appelé deux fois de suite
 */
LIBCOMM_EXPORT int log_destroy(log_phandle_t _this);

/**
 * @brief log_push ajout un message dans la queue
 *
 * @param _this handle du log
 * @param szMsg message (!= NULL), si plus grand que la taille LOG_MSG_LEN il sera tronqué (\0 compris)
 * @return état de l'opération
 * @retval 0 OK
 * @retval -1 erreur dans l'argument
 * @retval -2 plus de place dans la queue
 *
 */
LIBCOMM_EXPORT int log_push(log_phandle_t _this, const char * szMsg);


/**
 * @brief log_pop pop un message depuis la queue
 *
 * @param _this handle du log
 * @param szMsg un message (!=NULL), donc la taille du buffer est d'au moins LOG_MSG_LEN, 0 final compris
 * @return 0 pas de message
 * @return 1 un message poppé
 * @return -1 erreur d'argument
 */
LIBCOMM_EXPORT int log_pop(log_phandle_t _this, char * szMsg);

/**
 * @brief log_size retourne la taille du contenu en nombre de message
 * @param _this handle
 * @return taille en nombre de message
 */
LIBCOMM_EXPORT size_t log_getsize(log_phandle_t _this);

/**
 * @brief log_clear vide la file
 * @param _this
 */
LIBCOMM_EXPORT void log_clear(log_phandle_t _this);

// Si défini alors on compile aussi le test unitaire
#ifdef LOG_TEST
#warning "le code de test du log est activé"
int log_test(void);
#endif

#ifdef __cplusplus
} // extern "C"
#endif

#endif // LOG_H
