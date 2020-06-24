#ifndef __LED_595_H
#define __LED_595_H	 
#include "stm32f1xx_hal.h"
#include <rtthread.h>
#include <rtdef.h>
#include <rthw.h>

//////////////////////////////////////////////////////////////////////////////////	 

#define LATCH_LED_Pin GPIO_PIN_13
#define LATCH_LED_GPIO_Port GPIOC
#define CLK_LED_Pin GPIO_PIN_14
#define CLK_LED_GPIO_Port GPIOC
#define DI_LED_Pin GPIO_PIN_15
#define DI_LED_GPIO_Port GPIOC

static int Led_595_Init(void);//≥ı ºªØ
void Led_Set_16Bit(uint16_t value);

#endif
