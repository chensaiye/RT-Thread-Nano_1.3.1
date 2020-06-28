#ifndef __USART3_H
#define __USART3_H
#include "stm32f1xx_hal.h"
#define USART3_REC_LEN 64

#define USART3_RXOVER_WITH_TIM
#define USART3_TXSEND_WITH_DMA

extern UART_HandleTypeDef huart3;

#ifdef	USART3_RXOVER_WITH_TIM
	extern TIM_HandleTypeDef htim7;
#endif

#ifdef	USART3_TXSEND_WITH_DMA
	extern DMA_HandleTypeDef hdma_usart3_tx;
	static void MX_DMA_Init(void);
#endif

static int MX_USART3_UART_Init(void);
void HAL_UART_MspInit(UART_HandleTypeDef* huart); 


#endif


