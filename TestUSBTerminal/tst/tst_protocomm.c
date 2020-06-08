/*
 * tst_protocomm.c
 *
 *  Created on: 24 mai 2020
 *      Author: sli
 *
 *  Int�gration du protocole de communication
 *
 *  Au choix :
 *  - On recree les �changes � partir de l'API ll
 *  - On utilise le protocole de haut niveau avec un device appropri�
 *
 * En premi�re r�flexion pour le device:
 * 	- le read() serait sur une FIFO sur laquel Receive_FS fait ses pushs
 *	- Le write encapsule le transmit
 *
 */

#include "tst_select.h"

#ifdef TST_PROTOCOMM

#include "usbd_cdc_if.h"

int8_t _CDC_Receive_FS_user(uint8_t* Buf, uint32_t *Len)
{

}
#endif

