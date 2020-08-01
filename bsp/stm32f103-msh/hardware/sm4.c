#include "sm4.h"
/********************************************************************************/
//////////////////////////////////////////////////////////////////////////////////	
//4位拨码开关输入 驱动代码
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//STM32开发板
//CSY
//修改日期:2020/4/16
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 
//All rights reserved									   
//////////////////////////////////////////////////////////////////////////////////
/********************************************************************************/
//修改说明 
//V1.0 为适应RT-Thread V3.1.3
////////////////////////////////////////////////////////////////////////////////// 
/********************************************************************************/
#include "stm32f1xx_hal.h"
#include <rthw.h>
#include <rtthread.h>

//初始化并使能时钟		    
int SM4_Init(void)
{
		GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOC_CLK_ENABLE();
		GPIO_InitStruct.Pin   = SM_Pin1 | SM_Pin2 | SM_Pin3| SM_Pin4;
    GPIO_InitStruct.Mode  = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
		return 0;
}
//INIT_BOARD_EXPORT(SM4_Init);

uint8_t SM4_Get_Date(void)
{
	 uint8_t i=0xFF;
	 i = HAL_GPIO_ReadPin(SM_GPIO_Port,SM_Pin1) | (HAL_GPIO_ReadPin(SM_GPIO_Port,SM_Pin2)<<1) | \
			(HAL_GPIO_ReadPin(SM_GPIO_Port,SM_Pin3)<<2) |(HAL_GPIO_ReadPin(SM_GPIO_Port,SM_Pin4)<<3);
	 return (i&0xFF);
}



