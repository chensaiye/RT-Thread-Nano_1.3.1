#ifndef __TOF10120_H
#define __TOF10120_H
#include "iic.h"   
#include "stm32f1xx_hal.h"
#include <rtthread.h>
#include <rtdef.h>
#include <rthw.h>

#define TOF_ADDRESS1	0XA4		
#define REAL_TIME_DIS_REG	0x00
#define FILTERED_DIS_REG	0x04


void TOF10120_Init(void);
void TOF10120_Read(uint16_t ReadAddr,uint8_t *pBuffer,uint16_t NumToRead);
uint16_t TOF10120_Read_Distence(void);
#endif
















