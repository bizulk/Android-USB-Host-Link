/*
 * tst_iface.h
 *
 *  Created on: 24 mai 2020
 *      Author: sli
 *
 *  Définit l'interface sur l'API de test
 */

extern proto_Device_t monDevice;

#ifndef TST_IFACE_H_
#define TST_IFACE_H_

/** Boucle weak qui est appelée dans la boucle principale a la cadence d'1Hz
 *
 */
extern __weak void tst_loop_1hz(void);

/** Fonction callback sur la réception USB
 * Pour la description des arguments voir CDC_Receive_FS
 * */
extern __weak int8_t _CDC_Receive_FS_user(uint8_t* Buf, uint32_t *Len);

#endif /* TST_IFACE_H_ */
