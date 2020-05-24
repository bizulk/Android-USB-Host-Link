/** Envoi du message HelloWord USB sur le post USB.
 * 	Utilise la fonction weak de test pour que ce soit envoyé toutes les secondes
 * */
#include "tst_select.h"

#ifdef TST_HELLOWORD_USB
#include "usbd_cdc_if.h"

#define TST_SZ_HELLOW "Hello World \n\r"

void tst_loop_1hz(void)
{
	volatile uint8_t ret = 0; /* Juste pour observation en cas de debug */
	ret = CDC_Transmit_FS(TST_SZ_HELLOW, strlen(TST_SZ_HELLOW));
}
#endif
