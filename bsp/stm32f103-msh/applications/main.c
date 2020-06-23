/*
 * Copyright (c) 2006-2019, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-04-24     Roy_chen     First edition
 */

#include "stm32f1xx_hal.h"
#include <rtthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "led_595.h"
#include "tof10120.h" 
#define LD2_GPIO_PORT  GPIOA
#define LD2_PIN        GPIO_PIN_0

static void MX_GPIO_Init(void);


/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

int main(void)
{
    MX_GPIO_Init();
		TOF10120_Init();
    while (1)
    {
        HAL_GPIO_WritePin(LD2_GPIO_PORT, LD2_PIN, GPIO_PIN_SET);
        rt_thread_mdelay(500);
        HAL_GPIO_WritePin(LD2_GPIO_PORT, LD2_PIN, GPIO_PIN_RESET);
        rt_thread_mdelay(500);
				
				rt_kprintf("dis:%d mm\r\n", TOF10120_Read_Distence()); 
    }
}

static void MX_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

		__HAL_RCC_AFIO_CLK_ENABLE();//io复用使能
		__HAL_AFIO_REMAP_SWJ_NOJTAG();//关闭jtag，使能SWD，可以用SWD模式调试 ,使用jtag调试口做普通io时，必须使能io复用
    __HAL_RCC_GPIOA_CLK_ENABLE();
    HAL_GPIO_WritePin(LD2_GPIO_PORT, LD2_PIN, GPIO_PIN_RESET);

    GPIO_InitStruct.Pin   = LD2_PIN;
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(LD2_GPIO_PORT, &GPIO_InitStruct);
}


/**
 * This function use to test some drive demo
 * @param argc:the number of paras.
 * argv[0]:the name of function.
 * argv[1]:frist para
 * argv[2]:secend para
 */
void Led_Set_Test(int argc, char** argv)
{
	if(atoi(argv[1])==1)
		Led_Set_16Bit(0xaaaa);
	else if(atoi(argv[1])==2)
		Led_Set_16Bit(0x5555);
	else
		Led_Set_16Bit(0xFFFE);
	rt_kprintf("order:%s para1:%s\r\n",argv[0],argv[1]);
}
MSH_CMD_EXPORT(Led_Set_Test, Led_595_set test);
