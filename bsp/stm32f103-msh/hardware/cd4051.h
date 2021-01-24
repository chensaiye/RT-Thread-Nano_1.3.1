#ifndef __CD4051_H
#define __CD4051_H	 
#include "stm32f1xx_hal.h"
#include <rtthread.h>
#include <rtdef.h>
#include <rthw.h>
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK Mini STM32开发板
//LED驱动代码	   
//正点原子@ALIENTEK
//技术论坛:
//修改日期:
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	   
#define CD4051_A GPIO_PIN_13
#define CD4051_B GPIO_PIN_14
#define CD4051_C GPIO_PIN_15
#define CD4051_Port GPIOB

void CD4051_Init(void);//初始化
void CD4051_Ch_Set(uint8_t ch, uint8_t endis);//4051输出 ,ch通道 ，endis
void CD4051_Ch_Select(uint16_t ch);//通道选择
#endif

















