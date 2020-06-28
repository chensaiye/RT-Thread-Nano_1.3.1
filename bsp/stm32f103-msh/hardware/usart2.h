#ifndef __USART2_H
#define __USART2_H
#include "stm32f1xx_hal.h"

#define USART2_RXOVER_WITH_TIM
#define USART2_TXSEND_WITH_DMA

#define USART2_REC_LEN 64

extern UART_HandleTypeDef huart2;

#ifdef	USART2_RXOVER_WITH_TIM
	extern TIM_HandleTypeDef htim6;
#endif

#ifdef	USART2_TXSEND_WITH_DMA
	extern DMA_HandleTypeDef hdma_usart2_tx;
	static void MX_DMA_Init(void);
#endif


static int MX_USART2_UART_Init(void);
void HAL_UART_MspInit(UART_HandleTypeDef* huart); 

#endif





