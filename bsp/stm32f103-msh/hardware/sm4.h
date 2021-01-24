#ifndef __SM4_H
#define __SM4_H	 
#include "stm32f1xx_hal.h"
#include <rtthread.h>
#include <rtdef.h>
#include <rthw.h>

//////////////////////////////////////////////////////////////////////////////////	 
#define SM_Pin1 GPIO_PIN_0
#define SM_Pin2 GPIO_PIN_1
#define SM_Pin3 GPIO_PIN_2
#define SM_Pin4 GPIO_PIN_3
#define SM_GPIO_Port GPIOC

int SM4_Init(void);//≥ı ºªØ
uint8_t SM4_Get_Date(void);

#endif
