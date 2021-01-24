#ifndef __CD4051_CTL_H
#define __CD4051_CTL_H	 
#include "sys.h"
#include "cd4051.h"
#include "stm32f10x_conf.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK Mini STM32������
//LED��������	   
//����ԭ��@ALIENTEK
//������̳:
//�޸�����:
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	   

#define EEPROM_Vol			0x38  //192byte 0x38-0xf7
void Color_Temper_Init(void);
void write_temp_level(u16 temp,u16 level ,u16 ch0,u16 ch1,u16 ch2,u16 ch3);
void save_temp_level(void);
void write_dac2_level(u16 level ,u16 value);
void save_dac2_level(void);
#endif

















