#ifndef __CD4051_CTL_H
#define __CD4051_CTL_H	 
#include "sys.h"
#include "cd4051.h"
#include "stm32f10x_conf.h"
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

#define EEPROM_Vol			0x38  //192byte 0x38-0xf7
void Color_Temper_Init(void);
void write_temp_level(u16 temp,u16 level ,u16 ch0,u16 ch1,u16 ch2,u16 ch3);
void save_temp_level(void);
void write_dac2_level(u16 level ,u16 value);
void save_dac2_level(void);
#endif

















