#ifndef __LM75A_H
#define __LM75A_H
#include "iic_bus.h"  
#include "stm32f1xx_hal.h"
#include <rtthread.h>
#include <rtdef.h>
#include <rthw.h>
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;

#define LM75A_ADDRESS 0x90

#define Swap16(A)  {A = (((A & 0xff00) >> 8) | ((A & 0x00ff) << 8));}

HAL_StatusTypeDef LM75A_Read(uint16_t ReadAddr,uint8_t *pBuffer,uint16_t NumToRead);   	//��ָ����ַ��ʼ����ָ�����ȵ�����
static int LM75A_Init(void); //��ʼ��IIC
uint16_t LM75A_Read_Test(void);
#endif
















