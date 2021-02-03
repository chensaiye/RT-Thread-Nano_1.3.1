#include "cd4051.h"
#include "stm32f1xx_hal.h"
#include <rthw.h>
#include <rtthread.h>
/********************************************************************************/
//////////////////////////////////////////////////////////////////////////////////	
//LED 驱动代码
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//STM32开发板
//CSY
//修改日期:2015/10/09
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 启东应成医疗器械有限公司
//All rights reserved									   
//////////////////////////////////////////////////////////////////////////////////
/********************************************************************************/
//修改说明 

////////////////////////////////////////////////////////////////////////////////// 
/********************************************************************************/


//cd4051 IO初始化
void CD4051_Init(void)
{
   GPIO_InitTypeDef  GPIO_InitStructure;
	//cd4051端口设置
	 __HAL_RCC_GPIOB_CLK_ENABLE();
	  GPIO_InitStructure.Pin   = CD4051_A |CD4051_B |CD4051_C;
    GPIO_InitStructure.Mode  = GPIO_MODE_OUTPUT_PP;//推挽输出
    GPIO_InitStructure.Pull  = GPIO_NOPULL;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_LOW;
	 HAL_GPIO_Init(CD4051_Port, &GPIO_InitStructure);					 //根据设定参数初始化GPIOC
	 
}
INIT_BOARD_EXPORT(CD4051_Init);

//4051输出 ,ch通道 ，endis
//void CD4051_Ch_Set(uint8_t ch, uint8_t endis)
//{
//	uint16_t tp;
//	ch &= 0x07; //0~7值
//	if(endis)
//	{
//		tp = GPIO_ReadOutputData(GPIOB);
//		tp &= 0x1FFF;
//		tp = (ch<<13) | tp;
//		GPIO_Write(GPIOB,tp);
//	}	
//}

//void CD4051_Ch_Select(uint16_t ch)
//{
//	uint16_t tp;
//	ch &= 0x07; //0~7值
//	ch <<= 13;
//		
//		tp = GPIO_ReadOutputData(GPIOB);
//		tp &= 0xFFF8;
//		tp = ch | tp;
//		GPIO_Write(GPIOB,tp);	
//}

void CD4051_Ch_Select(uint16_t ch)
{
	ch &= 0x07; //0~7值
	if((ch & 0x01)==0)
		HAL_GPIO_WritePin(CD4051_Port,CD4051_A,GPIO_PIN_RESET);
	else
		HAL_GPIO_WritePin(CD4051_Port,CD4051_A,GPIO_PIN_SET);
	
	if((ch & 0x02)==0)
		HAL_GPIO_WritePin(CD4051_Port,CD4051_B,GPIO_PIN_RESET);
	else
		HAL_GPIO_WritePin(CD4051_Port,CD4051_B,GPIO_PIN_SET);
	
	if((ch & 0x04)==0)
		HAL_GPIO_WritePin(CD4051_Port,CD4051_C,GPIO_PIN_RESET);
	else
		HAL_GPIO_WritePin(CD4051_Port,CD4051_C,GPIO_PIN_SET);
}
