/* Auteur : Selso LIBERADO
 * Implémentation Interface de log pour ajouter des traces d'exécution dans la lib.
 * Pour répondre aussi au besoin de debug filaire sous android on propose une FIFO de messages
 * On peut sélectionner à la compilation un fonctionnement à base de printf/ FIFO
 * Dans le cas d'un printf null pas de fifo utilisé : la trace est directement affichée
 *
*/

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "log.h"


/// si défini, alors on utilisera une fifo, sinon on utilisera un printf
#define LOG_USE_FIFO

/// Handle du composant
/// Notons que begin est égal à first quand c'est vide
typedef struct log_handle
{
    uint32_t nbMsg; // Nombre de message enregistrable
    char ** ptMsg; // Fil de message
    char * begin; //  premier message
    char * end; // emplacement dispo du prochain message
} log_handle_t;

//-----------------------------------------------------------------------------
log_phandle_t log_create(uint32_t nbMsg)
{
    log_phandle_t _this = (log_phandle_t)malloc(sizeof (log_handle_t));
    // Pour la gestion des pointeurs il faut sacrifier un emplacement
    _this->ptMsg = (char **)malloc((nbMsg+1)* LOG_MSG_LEN);
    if( !_this->ptMsg)
    {
        free(_this);
        _this = NULL;
    }
    _this->nbMsg = nbMsg+1;
    _this->begin = (char *)_this->ptMsg;
    _this->end = _this->begin;
    return _this;
}

//-----------------------------------------------------------------------------
int log_destroy(log_phandle_t _this)
{
    assert(_this);
    free( _this->ptMsg);
    memset(_this, 0, sizeof(*_this));
    free(_this);
    return 0;
}

//-----------------------------------------------------------------------------
size_t log_getsize(log_phandle_t _this)
{
    assert(_this);
    size_t size = 0;
    // cas initial
    if( _this->begin == _this->end )
        return 0;
    if(_this->end > _this->begin)
    {
           size = (_this->end - _this->begin)/LOG_MSG_LEN;
    }
    // buffer tournant : on a fait un tour du buffer
    else
    {
        size = _this->nbMsg - (_this->begin - _this->end)/LOG_MSG_LEN;
    }
    return size;
}

size_t log_getfree(log_phandle_t _this)
{
    assert(_this);
    // il faut décrementer de 1 car un bloc est sacrifié
    return _this->nbMsg - log_getsize(_this) -1;
}

//-----------------------------------------------------------------------------
int log_push(log_phandle_t _this, const char * szMsg)
{
   // Detection de la capacité :
    assert(_this && szMsg);

    if(!log_getfree(_this)) return -2;

    strncpy(_this->end, szMsg, LOG_MSG_LEN);
    // Securité : on force à zéro
    _this->end[LOG_MSG_LEN-1] =0;
    _this->end += LOG_MSG_LEN;
    // Arrivé à la fin : on retourne au départ
    if( _this->end >= ((char*)_this->ptMsg + _this->nbMsg * LOG_MSG_LEN ))
    {
        _this->end = (char*)_this->ptMsg;
    }
    return 0;
}


//-----------------------------------------------------------------------------
int log_pop(log_phandle_t _this, char * szMsg)
{
    assert(_this && szMsg);
    if( ! log_getsize(_this) )
        return 0;

    strncpy(szMsg, _this->begin, LOG_MSG_LEN);
    _this->begin += LOG_MSG_LEN;
    // Arrivé à la fin : on retourne au départ
    if( _this->begin >= ((char*)_this->ptMsg + _this->nbMsg * LOG_MSG_LEN ))
    {
        _this->begin = (char*)_this->ptMsg;
    }

    return 1;
}

//-----------------------------------------------------------------------------
void log_clear(log_phandle_t _this)
{
    assert(_this);
    _this->begin = _this->end;
}

//-----------------------------------------------------------------------------
static log_phandle_t _log_global_this = NULL;
int log_global_create(uint32_t nbMsg)
{
    _log_global_this = log_create(nbMsg);
    return (_log_global_this!=NULL) ? 0 : -1;
}

//-----------------------------------------------------------------------------
int log_global_destroy(void)
{
    return log_destroy(_log_global_this);
}

//-----------------------------------------------------------------------------
int log_global_push(const char * szMsg)
{
    return log_push(_log_global_this, szMsg);
}

//-----------------------------------------------------------------------------
int log_global_pop(char * szMsg)
{
    return log_pop(_log_global_this, szMsg);
}

//-----------------------------------------------------------------------------
size_t log_global_getsize(void)
{
    return log_getsize(_log_global_this);
}

//-----------------------------------------------------------------------------
LIBCOMM_EXPORT size_t log_global_getfree(void)
{
    return log_getfree(_log_global_this);
}

//-----------------------------------------------------------------------------
void log_global_clear(void)
{
    return log_clear(_log_global_this);
}

//-----------------------------------------------------------------------------
#ifdef LOG_TEST

int log_test(void)
{
#define TST_NB_MSG 5
    static char _tszMsg[TST_NB_MSG][LOG_MSG_LEN] = {{0}};
    char szMsgtst[LOG_MSG_LEN] = {0};
    int i;
    int ret;
    // Initialisation avec des message différents pour contrôler l'ordre
    for( i = 0; i < TST_NB_MSG ; i++)
    {
        snprintf((char*)_tszMsg[i], LOG_MSG_LEN, "msg%d", i);
    }

    log_phandle_t loghdle = log_init(TST_NB_MSG);
    assert(loghdle);

    // TEST 1 remplir la file et la vider et contrôler l'ordre
    for( i = 0; i < TST_NB_MSG ; i++)
    {
        log_push(loghdle, _tszMsg[i]);
        // Au passage on contrôle la taille
        assert( log_getsize(loghdle) == (size_t)i+1 );
        assert( log_getfree(loghdle) == (size_t)(TST_NB_MSG - i -1) );
    }
    for( i = 0; i < TST_NB_MSG ; i++)
    {
        ret = log_pop(loghdle, szMsgtst);
        assert(ret != 0);
        assert(strcmp(szMsgtst, _tszMsg[i]) == 0);
    }

    // TEST2 : on dépasse la capacité de la pile
    for( i = 0; i < TST_NB_MSG ; i++)
    {
        log_push(loghdle, _tszMsg[i]);
    }
    ret = log_push(loghdle, _tszMsg[i]);
    assert( ret = -2 );

    // TEST4 : test du clear & on pope alors que c'est vide
    log_clear(loghdle);
    assert( log_getsize(loghdle) == 0 );
    ret = log_pop(loghdle, szMsgtst);
    assert( ret == 0 );

    log_destroy(loghdle);

    return 0;
}
#endif
