#ifndef __USART3_H
#define __USART3_H
#include "stm32f1xx_hal.h"
extern UART_HandleTypeDef huart3;

void MX_USART3_UART_Init(void);
void HAL_UART_MspInit(UART_HandleTypeDef* huart); 
void HAL_UART_MspDeInit(UART_HandleTypeDef* huart);

#endif


