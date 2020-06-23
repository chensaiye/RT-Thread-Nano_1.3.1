#ifndef __USART1_H
#define __USART1_H
#include "stm32f1xx_hal.h"
extern UART_HandleTypeDef huart1;

void MX_USART1_UART_Init(void);
void HAL_UART_MspInit(UART_HandleTypeDef* huart); 
void HAL_UART_MspDeInit(UART_HandleTypeDef* huart);

#endif



