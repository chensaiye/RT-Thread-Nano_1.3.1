#ifndef __IIC_BUS_H
#define __IIC_BUS_H
#include "stm32f1xx_hal.h"
#include <rtthread.h>
#include <rtdef.h>
#include <rthw.h>

//IIC所有操作函数
void MX_I2C1_Init(void);
void MX_I2C2_Init(void);
//IIC 写
uint8_t I2C_Master_BufferWrite(I2C_TypeDef * I2Cx, uint8_t* pBuffer, uint32_t NumByteToWrite, uint8_t SlaveAddress,uint8_t WriteAddress);
//IIC 读
uint8_t I2C_Master_BufferRead(I2C_TypeDef * I2Cx, uint8_t* pBuffer, uint32_t NumByteToRead, uint8_t SlaveAddress,uint8_t ReadAddress);
 
#endif
















