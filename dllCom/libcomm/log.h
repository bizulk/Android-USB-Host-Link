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
 *
 * Deux fonctionnements :
 * - le fonctionnement qui permet d'avoir plusieurs instances de log (on passe le handle en paramètre)
 * - un fonctionnement simplifié qui crée une instance globale caché.
 *
*/

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdint.h>

#include "libcomm_global.h"

/******************************************************************************
 * TYPES & VARIABLES
******************************************************************************/

/// Taille moyenne d'un message
/// \sa log_push
///  Vous pouvez modifier ce paramètre, il faudrat recompiler la lib
#define LOG_MSG_LEN 100

/// si défini, alors on utilisera une fifo, sinon on utilisera un printf
/// TODO
//#define LOG_USE_FIFO
#define LOG_USE_CONSOLE
#ifdef LOG_USE_FIFO
#define LOG(fmt, args...) LOG_PUSH(_this, fmt##args)
#elif defined(LOG_USE_GLOBAL_FIFO)
#define LOG(fmt, args...) LOG_GLOBAL_PUSH(fmt##args)
#elif defined(LOG_USE_CONSOLE)
#define LOG(fmt, args...) printf(fmt,##args)
#else
#define LOG(fmt, args...)
#endif

/// Instance de notre log
typedef struct log_handle * log_phandle_t;


/******************************************************************************
 * PROTO - multi-instance de log
******************************************************************************/

/** Helper pour envoyer un message de log d'exécution
 * Ajoute l'emplacement 'fonction' et le numéro de ligne au message
*/
#define LOG_PUSH(_this, fmt, args...) do\
{\
    char szMsg[100]={0};\
    snprintf( szMg, LOG_MSG_LEN, "%s:%d -"##fmt, __FUNCTION__, __LINE__ -2,##args);\
    log_push(_this, szMg);\
 } while(0);

/** Helper pour envoyer un message de log d'exécution
 * Ajoute l'emplacement 'fonction' et le numéro de ligne au message
*/
#define LOG_GLOBAL_PUSH(fmt, args...) do\
{\
    char szMsg[100]={0};\
    snprintf( szMg, LOG_MSG_LEN, "%s:%d -"##fmt, __FUNCTION__, __LINE__ -2,##args);\
    log_global_push(szMg);\
 } while(0);


/** Création de notre log
    @param nbMsg nombre maximal de message
    @return retour d'initialisation, NULL si erreur
*/
LIBCOMM_EXPORT log_phandle_t log_create(uint32_t nbMsg);

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
 * @brief log_getfree retourne le nombre d'éléments disponible
 * @param _this
 * @return [0-, nbMsg]
 */
LIBCOMM_EXPORT size_t log_getfree(log_phandle_t _this);

/**
 * @brief log_clear vide la file
 * @param _this
 */
LIBCOMM_EXPORT void log_clear(log_phandle_t _this);

/******************************************************************************
 * PROTO - log unique (instance interne)
******************************************************************************/

/**
 * @brief log_global_init Creation d'un instance globale de log
 * @param nbMsg nombre de message souhaité
 * @return 0 OK, !0 erreur
 * Si appelée deux fois de suite, le seconde appel est ignoré
 */
LIBCOMM_EXPORT int log_global_create(uint32_t nbMsg);

/**
 * @brief log_destroy libération des ressources
 * @return 0 OK, -1 erreur interne ou pas d'init appelé
 *
 * \warning comportement indéterminé si appelé deux fois de suite
 */
LIBCOMM_EXPORT int log_global_destroy(void);

/**
 * @brief log_global_push
 * @param szMsg
 * @return
 */
LIBCOMM_EXPORT int log_global_push(const char * szMsg);

/**
 * @brief log_global_pop
 * @param szMsg
 * @return
 */
LIBCOMM_EXPORT int log_global_pop(char * szMsg);

/**
 * @brief log_global_getsize
 * @return
 */
LIBCOMM_EXPORT size_t log_global_getsize(void);

/**
 * @brief log_global_getfree
 * @return
 */
LIBCOMM_EXPORT size_t log_global_getfree(void);

/**
 * @brief log_global_clear
 */
LIBCOMM_EXPORT void log_global_clear(void);

// Si défini alors on compile aussi le test unitaire
#ifdef LOG_TEST
#warning "le code de test du log est activé"
int log_test(void);
#endif

#ifdef __cplusplus
} // extern "C"
#endif

#endif // LOG_H
