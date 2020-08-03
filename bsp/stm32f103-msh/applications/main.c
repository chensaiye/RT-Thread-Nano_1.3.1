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
#include "24cxx.h" 
#include "button.h"
#include "pwm_tim3.h"
#include "oled.h"
#include "main_control.h"

#define RUN_GPIO_PORT  GPIOB
#define RUN_PIN        GPIO_PIN_12
#define LED0_Pin GPIO_PIN_4
#define LED0_GPIO_Port GPIOA
#define LED1_Pin GPIO_PIN_1
#define LED1_GPIO_Port GPIOB

/* 定时器的控制块 */
static rt_timer_t timer1;
static rt_timer_t timer2;

extern uint8_t Channels_RIR[CHANNEL_RIR_MAX];

static void MX_GPIO_Init(void);
static void MX_EXTI_GPIO_Init(void);

extern void Panel_Init(void);
extern void backup_data(void);
extern void Led_Blink(void);

extern int thread_usart3_init(void);
extern int thread_pwm_start(void);
extern int thread_nrf_init(void);
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
void PowDown_Test_Init(void)
{

	PWR_PVDTypeDef PWR_PVD_INIT_Struct={0};
	/* Enable PWR and BKP clock */
	__HAL_RCC_BKP_CLK_ENABLE();
	__HAL_RCC_PWR_CLK_ENABLE();
	
	/* Configure EXTI Line to generate an interrupt on falling edge */
		
	/* NVIC configuration */
	/* PVD_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(PVD_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(PVD_IRQn);
	
	PWR_PVD_INIT_Struct.Mode = PWR_PVD_MODE_IT_RISING;//why is rising???
	PWR_PVD_INIT_Struct.PVDLevel =  PWR_PVDLEVEL_7;
	HAL_PWR_ConfigPVD(&PWR_PVD_INIT_Struct);
	HAL_PWR_EnablePVD();
				 
}

/**
  * @brief This function handles PVD interrupt through EXTI line 16.
  */
void PVD_IRQHandler(void)
{
  /* USER CODE BEGIN PVD_IRQn 0 */
	HAL_GPIO_WritePin(GP1_POW_GPIO_Port, GP1_POW_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(QJ_POW_GPIO_Port, QJ_POW_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LED0_GPIO_Port, LED0_Pin, GPIO_PIN_SET);
	rt_enter_critical();//调度器上锁
	backup_data();
  /* USER CODE END PVD_IRQn 0 */
  HAL_PWR_PVD_IRQHandler();
  /* USER CODE BEGIN PVD_IRQn 1 */
	HAL_GPIO_WritePin(LED0_GPIO_Port, LED0_Pin, GPIO_PIN_RESET);
	rt_exit_critical();//调度器解锁	
  /* USER CODE END PVD_IRQn 1 */
}


/* 定时器1超时函数 */
static void time1_out(void *parameter)
{
 // HAL_GPIO_TogglePin(RUN_GPIO_PORT, RUN_PIN);
	Led_Blink();
}

/* 定时器1超时函数 */
static void time2_out(void *parameter)
{
		uint16_t distance;
		distance = TOF10120_Read_Scan();
		if((distance>450) && Channels_RIR[6])
			bt_rir_7(BUTTON_LONG_RELEASE);
		if((distance<=450) && Channels_RIR[6]==0)
			bt_rir_7(BUTTON_LONG_PRESSED);
}

static void times_init(void)
{
		/* 创建定时器1  周期定时器 */
    timer1 = rt_timer_create("timer1", time1_out,
                             RT_NULL, 20,
                             RT_TIMER_FLAG_PERIODIC);
    /* 启动定时器1 */
    if (timer1 != RT_NULL)
        rt_timer_start(timer1);
		
		/* 创建定时器2  周期定时器 */
    timer2 = rt_timer_create("timer2", time2_out,
                             RT_NULL, 200,
                             RT_TIMER_FLAG_PERIODIC);
    /* 启动定时器1 */
    if (timer2 != RT_NULL)
        rt_timer_start(timer2);
		
}
	

int main(void)
{
		TOF10120_Init();
    MX_GPIO_Init();
		Panel_Init();
		
	
		thread_button_init();
		thread_pwm_start();
		thread_nrf_init();
		oled_thd_start();
		//thread_usart3_init();
		
		rt_thread_mdelay(200);
		MX_EXTI_GPIO_Init();
		//PowDown_Test_Init();
	
		times_init();
		
		while (1)
    {
				HAL_GPIO_TogglePin(RUN_GPIO_PORT, RUN_PIN);
				rt_thread_mdelay(500);
		}
}
/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

		__HAL_RCC_AFIO_CLK_ENABLE();//io复用使能
		__HAL_AFIO_REMAP_SWJ_NOJTAG();//关闭jtag，使能SWD，可以用SWD模式调试 ,使用jtag调试口做普通io时，必须使能io复用
    __HAL_RCC_GPIOB_CLK_ENABLE();
    HAL_GPIO_WritePin(RUN_GPIO_PORT, RUN_PIN, GPIO_PIN_RESET);

    GPIO_InitStruct.Pin   = RUN_PIN;
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(RUN_GPIO_PORT, &GPIO_InitStruct);
		
		GPIO_InitStruct.Pin 	= LED0_Pin;
		HAL_GPIO_Init(LED0_GPIO_Port, &GPIO_InitStruct);
		GPIO_InitStruct.Pin 	= LED1_Pin;
		HAL_GPIO_Init(LED1_GPIO_Port, &GPIO_InitStruct);
		
		HAL_GPIO_WritePin(LED0_GPIO_Port, LED0_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_EXTI_GPIO_Init(void)
{	
	GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOB_CLK_ENABLE();
	
/*Configure GPIO pin : PB2 */
  GPIO_InitStruct.Pin = GPIO_PIN_2;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	
 /* EXTI interrupt init*/

	HAL_NVIC_SetPriority(EXTI2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI2_IRQn);
}

/**
  * @brief This function handles EXTI line2 interrupt.
  */
void EXTI2_IRQHandler(void)
{
	
  /* USER CODE BEGIN EXTI2_IRQn 0 */
	if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2)==GPIO_PIN_SET)
	{
		rt_enter_critical();//调度器上锁
		HAL_GPIO_WritePin(GP1_POW_GPIO_Port, GP1_POW_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(QJ_POW_GPIO_Port, QJ_POW_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);
		/* USER CODE END EXTI2_IRQn 0 */
		HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_2);
		/* USER CODE BEGIN EXTI2_IRQn 1 */
		//rt_thread_mdelay(10);
		backup_data();
		HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);
		rt_exit_critical();//调度器解锁
		/* USER CODE END EXTI2_IRQn 1 */
	}
	else
		HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_2);
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
//MSH_CMD_EXPORT(Led_Set_Test, Led_595_set test);
