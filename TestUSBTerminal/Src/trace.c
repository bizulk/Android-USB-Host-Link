/*
 * trace.c
 *
 *  Created on: 6 avr. 2020
 *      Author: sli
 */

/* Allow to use printf over ST-Link debug
 *
 */

#include "stm32f4xx.h"

#define WRITE_ON_USART3

#ifdef WRITE_ON_ITM
int _write(int file, char *ptr, int len)
{
	int DataIdx;

	for (DataIdx = 0; DataIdx < len; DataIdx++)
	{
	   ITM_SendChar( *ptr++ );
	}

	return len;
}
#else if defined(WRITE_ON_USART3)
#include "usart.h"

int _write(int file, char *ptr, int len)
{
	if( HAL_UART_Transmit(&huart3, (uint8_t*)ptr, len, 1000) == HAL_OK)
		return len;

	return 0;
}
#endif
