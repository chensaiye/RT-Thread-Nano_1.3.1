#include "led_595.h"
/********************************************************************************/
//////////////////////////////////////////////////////////////////////////////////	
//74HC595控制的LED输出 驱动代码
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


//#define HC595_LATCH     	GPIO_PIN_8
//#define HC595_CLK		     	GPIO_PIN_11
//#define HC595_DI		     	GPIO_PIN_12

//#define HC595_LATCH  GET_PIN(B, 3)
//#define HC595_CLK		GET_PIN(B, 4)
//#define HC595_DI	  GET_PIN(B, 5)

//初始化PB5和PE5为输出口.并使能这两个口的时钟		    
static int Led_595_Init(void)
{
		GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOC_CLK_ENABLE();
		GPIO_InitStruct.Pin   = LATCH_LED_Pin | CLK_LED_Pin | DI_LED_Pin;
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
		return 0;
}
INIT_BOARD_EXPORT(Led_595_Init);

//写595数据发光二极管数据
void Led_Set(uint8_t value)
{
	uint8_t i;
	HAL_GPIO_WritePin(LATCH_LED_GPIO_Port,LATCH_LED_Pin, GPIO_PIN_RESET);
	rt_hw_us_delay(5);
	for( i=0; i<8; i++ )
	{
		HAL_GPIO_WritePin(GPIOC,CLK_LED_Pin, GPIO_PIN_RESET);
		rt_hw_us_delay(5);
   	   
		if( value & (0x80 >> i))
			HAL_GPIO_WritePin(GPIOC,DI_LED_Pin, GPIO_PIN_SET);
		else
			HAL_GPIO_WritePin(GPIOC,DI_LED_Pin, GPIO_PIN_RESET);
		rt_hw_us_delay(5);
		HAL_GPIO_WritePin(GPIOC,CLK_LED_Pin, GPIO_PIN_SET);	
   	rt_hw_us_delay(5);
	}
	HAL_GPIO_WritePin(LATCH_LED_GPIO_Port,LATCH_LED_Pin, GPIO_PIN_SET);
}


void Led_Set_16Bit(uint16_t value)
{
	uint8_t i;
	//value = 0xA0A0;
	HAL_GPIO_WritePin(LATCH_LED_GPIO_Port,LATCH_LED_Pin, GPIO_PIN_RESET);
	rt_hw_us_delay(5);
	for( i=0; i<16; i++ )
	{
		HAL_GPIO_WritePin(GPIOC,CLK_LED_Pin, GPIO_PIN_RESET);
		rt_hw_us_delay(5);
   	   
		if( value & (0x8000 >> i))
			HAL_GPIO_WritePin(GPIOC,DI_LED_Pin, GPIO_PIN_SET);
		else
			HAL_GPIO_WritePin(GPIOC,DI_LED_Pin, GPIO_PIN_RESET);
		rt_hw_us_delay(5);
		HAL_GPIO_WritePin(GPIOC,CLK_LED_Pin, GPIO_PIN_SET);	
   	rt_hw_us_delay(5);
	}
	HAL_GPIO_WritePin(LATCH_LED_GPIO_Port,LATCH_LED_Pin, GPIO_PIN_SET);
}



