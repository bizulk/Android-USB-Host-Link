/*
 * tst_usb_echo.c
 *
 *  Created on: 24 mai 2020
 *      Author: sli
 *
 *  Réalise un écho des caractères reçu, pour tester avec un terminal PC
 */

#include "tst_select.h"

#ifdef TST_USB_ECHO

#include "usbd_cdc_if.h"

int8_t _CDC_Receive_FS_user(uint8_t* Buf, uint32_t *Len)
{
	// Echo the received chars
  CDC_Transmit_FS(Buf, *Len);
  // message de debug ( et de tests de l'implémentation du lien de debug)
  printf("received %lu char\n", *Len);
  return (USBD_OK);
}

#endif
