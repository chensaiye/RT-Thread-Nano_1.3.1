#ifndef __IIC_H
#define __IIC_H
#include "stm32f1xx_hal.h"
#include <rtthread.h>
#include <rtdef.h>
#include <rthw.h>

//IIC���в�������
void I2C1_Init(void);
void I2C2_Init(void);
//IIC д
uint8_t I2C_Master_BufferWrite(I2C_TypeDef * I2Cx, uint8_t* pBuffer, uint32_t NumByteToWrite, uint8_t SlaveAddress,uint8_t WriteAddress);
//IIC ��
uint8_t I2C_Master_BufferRead(I2C_TypeDef * I2Cx, uint8_t* pBuffer, uint32_t NumByteToRead, uint8_t SlaveAddress,uint8_t ReadAddress);
 
#endif
















