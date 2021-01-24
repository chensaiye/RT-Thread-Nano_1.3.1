#ifndef __TOF10120_H
#define __TOF10120_H
#include "iic_bus.h"   
#include "stm32f1xx_hal.h"
#include <rtthread.h>
#include <rtdef.h>
#include <rthw.h>

#define TOF1010_EN GPIO_PIN_12
#define TOF1010_EN_Port GPIOA

#define TOF_ADDRESS_REG	0X0F	
#define TOF_ADDRESS1	0XA4	

#define REAL_TIME_DIS_REG	0x00
#define FILTERED_DIS_REG	0x04


int TOF10120_Init(void);
HAL_StatusTypeDef TOF10120_Write(uint16_t WriteAddr,uint8_t *pBuffer,uint16_t NumToWrite);
HAL_StatusTypeDef TOF10120_Read(uint16_t ReadAddr,uint8_t *pBuffer,uint16_t NumToRead);
uint16_t TOF10120_Read_Distence(void);
uint16_t TOF10120_Read_Scan(void);
int tof_read_start(void);
#endif
















