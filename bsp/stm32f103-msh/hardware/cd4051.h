#ifndef __CD4051_H
#define __CD4051_H	 
#include "stm32f1xx_hal.h"
#include <rtthread.h>
#include <rtdef.h>
#include <rthw.h>
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
#define CD4051_A GPIO_PIN_13
#define CD4051_B GPIO_PIN_14
#define CD4051_C GPIO_PIN_15
#define CD4051_Port GPIOB

void CD4051_Init(void);//��ʼ��
void CD4051_Ch_Set(uint8_t ch, uint8_t endis);//4051��� ,chͨ�� ��endis
void CD4051_Ch_Select(uint16_t ch);//ͨ��ѡ��
#endif

















