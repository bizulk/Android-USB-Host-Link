/*
 * tst_protocomm.c
 *
 *  Created on: 24 mai 2020
 *      Author: sli
 *
 *  Intégration du protocole de communication
 *
 *  Au choix :
 *  - On recree les échanges à partir de l'API ll
 *  - On utilise le protocole de haut niveau avec un device approprié
 *
 * En première réflexion pour le device:
 * 	- le read() serait sur une FIFO sur laquel Receive_FS fait ses pushs
 *	- Le write encapsule le transmit
 *
 */
#include "tst_select.h"

extern proto_Device_t monDevice;

#ifdef TST_PROTOCOMM

#include "usbd_cdc_if.h"
<<<<<<< Updated upstream
=======
#include "protocomm_slave.h"
#include "device_stm32.h"
#include "protocomm_slave.h"

static proto_Device_t _stm32dev;
static proto_hdle_t * _protoSlave;

#define MY_TABLE_LEN 5

/** Implémentation des registres côté slave : une table "mémoire" */
static uint8_t _myTable[MY_TABLE_LEN] = {0};

/*** Callback d'appel du protocole pour traiter les commandes
 *
 */
static int slave_receive(void* userdata, proto_Command_t command, proto_frame_data_t* args_inout);


void tst_init(void)
{
	_stm32dev = devstm32_create();
	_protoSlave = proto_slave_create(_stm32dev, slave_receive, NULL);
}
>>>>>>> Stashed changes

int8_t _CDC_Receive_FS_user(uint8_t* Buf, uint32_t *Len)
{
	// TODO
	devstm32_pushBytes(monDevice, Buf, Len);
}


int slave_receive(void* userdata, proto_Command_t command, proto_frame_data_t* args_inout) {
	int ret = 0;
    UNUSED(userdata);
    switch (command) {
    case proto_CMD_SET: // quand le MASTER demande de changer une valeur
        if (args_inout->req.reg < EMULSLAVE_NB_REGS) {
            args_inout->req.reg = args_inout->req.value;
        } else
            ret = -1;
        break;

    case proto_CMD_GET: // quand le MASTER demande d'accÃ©der Ã  une valeur
        if (args_inout->req.reg < EMULSLAVE_NB_REGS)
        	args_inout->reg_value = args_inout->req.reg;
        else
            ret = -1;
        break;

    default:
        ret = -1;
        break;
    }
#endif

