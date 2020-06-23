#ifndef __SPI_BUS_H
#define __SPI_BUS_H
#include "stm32f1xx_hal.h"
#include <rtthread.h>
#include <rtdef.h>
#include <rthw.h>

//SPI所有操作函数
void MX_SPI1_Init(void);
void HAL_SPI_MspInit(SPI_HandleTypeDef* hspi);

#endif
















