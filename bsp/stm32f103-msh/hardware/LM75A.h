#ifndef __LM75A_H
#define __LM75A_H
#include "iic_bus.h"  
#include "stm32f1xx_hal.h"
#include <rtthread.h>
#include <rtdef.h>
#include <rthw.h>
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途

#define LM75A_ADDRESS 0x90

#define Swap16(A)  {A = (((A & 0xff00) >> 8) | ((A & 0x00ff) << 8));}

HAL_StatusTypeDef LM75A_Read(uint16_t ReadAddr,uint8_t *pBuffer,uint16_t NumToRead);   	//从指定地址开始读出指定长度的数据
static int LM75A_Init(void); //初始化IIC
uint16_t LM75A_Read_Test(void);
#endif
















