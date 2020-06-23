/*
 * Copyright (c) 2006-2019, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2017-07-24     Tanek        the first version
 * 2018-11-12     Ernest Chen  modify copyright
 */

#include "stm32f1xx_hal.h"
#include <rthw.h>
#include <rtthread.h>

#define _SCB_BASE       (0xE000E010UL)
#define _SYSTICK_CTRL   (*(rt_uint32_t *)(_SCB_BASE + 0x0))
#define _SYSTICK_LOAD   (*(rt_uint32_t *)(_SCB_BASE + 0x4))
#define _SYSTICK_VAL    (*(rt_uint32_t *)(_SCB_BASE + 0x8))
#define _SYSTICK_CALIB  (*(rt_uint32_t *)(_SCB_BASE + 0xC))
#define _SYSTICK_PRI    (*(rt_uint8_t  *)(0xE000ED23UL))

// Updates the variable SystemCoreClock and must be called
// whenever the core clock is changed during program execution.
extern void SystemCoreClockUpdate(void);
extern void Error_Handler(void);
// Holds the system core clock, which is the system clock
// frequency supplied to the SysTick timer and the processor
// core clock.
extern uint32_t SystemCoreClock;



static uint32_t _SysTick_Config(rt_uint32_t ticks)
{
    if ((ticks - 1) > 0xFFFFFF)
    {
        return 1;
    }

    _SYSTICK_LOAD = ticks - 1;
    _SYSTICK_PRI = 0xFF;
    _SYSTICK_VAL  = 0;
    _SYSTICK_CTRL = 0x07;

    return 0;
}

/**
 * This function will delay for some us.
 *
 * @param us the delay time of us
 */
void rt_hw_us_delay(rt_uint32_t us)
{
    rt_uint32_t start, now, delta, reload, us_tick;
    start = SysTick->VAL;
    reload = SysTick->LOAD;
    us_tick = SystemCoreClock / 1000000UL;
    do {
        now = SysTick->VAL;
        delta = start > now ? start - now : reload + start - now;
    } while(delta < us_tick * us);
}

#if defined(RT_USING_USER_MAIN) && defined(RT_USING_HEAP)
#define RT_HEAP_SIZE 1024
static uint32_t rt_heap[RT_HEAP_SIZE];  
RT_WEAK void *rt_heap_begin_get(void)
{
    return rt_heap;
}

RT_WEAK void *rt_heap_end_get(void)
{
    return rt_heap + RT_HEAP_SIZE;
}
#endif

/**
 * This function will initial your board.
 */
void rt_hw_board_init()
{
    /* System Clock Update */
    SystemCoreClockUpdate();

    /* System Tick Configuration */
    _SysTick_Config(SystemCoreClock / RT_TICK_PER_SECOND);

    /* Call components board initial (use INIT_BOARD_EXPORT()) */
#ifdef RT_USING_COMPONENTS_INIT
    rt_components_board_init();
#endif

#if defined(RT_USING_USER_MAIN) && defined(RT_USING_HEAP)
    rt_system_heap_init(rt_heap_begin_get(), rt_heap_end_get());
#endif
}

void SysTick_Handler(void)
{
    /* enter interrupt */
    rt_interrupt_enter();

    rt_tick_increase();

    /* leave interrupt */
    rt_interrupt_leave();
}

static UART_HandleTypeDef UartHandle;
static int uart_init(void)
{
    UartHandle.Instance = USART1;
    UartHandle.Init.BaudRate = 115200;
    UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
    UartHandle.Init.StopBits = UART_STOPBITS_1;
    UartHandle.Init.Parity = UART_PARITY_NONE;
    UartHandle.Init.Mode = UART_MODE_TX_RX;
    UartHandle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    UartHandle.Init.OverSampling = UART_OVERSAMPLING_16;

    if (HAL_UART_Init(&UartHandle) != HAL_OK)
    {
        while (1);
    }
    return 0;
}
INIT_BOARD_EXPORT(uart_init);

void rt_hw_console_output(const char *str)
{
    rt_size_t i = 0, size = 0;
    char a = '\r';

    __HAL_UNLOCK(&UartHandle);

    size = rt_strlen(str);
    for (i = 0; i < size; i++)
    {
        if (*(str + i) == '\n')
        {
            HAL_UART_Transmit(&UartHandle, (uint8_t *)&a, 1, 1);
        }
        HAL_UART_Transmit(&UartHandle, (uint8_t *)(str + i), 1, 1);
    }
}

char rt_hw_console_getchar(void)
{
    int ch = -1;

    if (__HAL_UART_GET_FLAG(&UartHandle, UART_FLAG_RXNE) != RESET)
    {
        ch = UartHandle.Instance->DR & 0xff;
    }
    else
    {
        rt_thread_mdelay(10);
    }
    return ch;
}

//#define USART_TX_Pin GPIO_PIN_2
//#define USART_TX_GPIO_Port GPIOA
//#define USART_RX_Pin GPIO_PIN_3
//#define USART_RX_GPIO_Port GPIOA

void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
		if (huart->Instance == USART1)
    {
			/* USER CODE BEGIN USART1_MspInit 0 */

			/* USER CODE END USART1_MspInit 0 */
				/* Peripheral clock enable */
				__HAL_RCC_USART1_CLK_ENABLE();
			
				__HAL_RCC_GPIOA_CLK_ENABLE();
				/**USART1 GPIO Configuration    
				PA9     ------> USART1_TX
				PA10     ------> USART1_RX 
				*/
				GPIO_InitStruct.Pin = GPIO_PIN_9;
				GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
				GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
				HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

				GPIO_InitStruct.Pin = GPIO_PIN_10;
				GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
				GPIO_InitStruct.Pull = GPIO_NOPULL;
				HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

			/* USER CODE BEGIN USART1_MspInit 1 */

			/* USER CODE END USART1_MspInit 1 */
			
		}
    else if (huart->Instance == USART2)
    {
			/* USER CODE BEGIN USART2_MspInit 0 */

			/* USER CODE END USART2_MspInit 0 */
				/* Peripheral clock enable */
				__HAL_RCC_USART2_CLK_ENABLE();
			
				__HAL_RCC_GPIOA_CLK_ENABLE();
				/**USART2 GPIO Configuration    
				PA2     ------> USART2_TX
				PA3     ------> USART2_RX 
				*/
				GPIO_InitStruct.Pin = GPIO_PIN_2;
				GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
				GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
				HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

				GPIO_InitStruct.Pin = GPIO_PIN_3;
				GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
				GPIO_InitStruct.Pull = GPIO_NOPULL;
				HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

			/* USER CODE BEGIN USART2_MspInit 1 */
//        GPIO_InitStruct.Pin = USART_TX_Pin | USART_RX_Pin;
//        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
//        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
//        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
			/* USER CODE END USART2_MspInit 1 */
			}
		else if(huart->Instance==USART3)
		{
		/* USER CODE BEGIN USART3_MspInit 0 */

		/* USER CODE END USART3_MspInit 0 */
			/* Peripheral clock enable */
			__HAL_RCC_USART3_CLK_ENABLE();
		
			__HAL_RCC_GPIOC_CLK_ENABLE();
			/**USART3 GPIO Configuration    
			PC10     ------> USART3_TX
			PC11     ------> USART3_RX 
			*/
			GPIO_InitStruct.Pin = GPIO_PIN_10;
			GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
			GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
			HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

			GPIO_InitStruct.Pin = GPIO_PIN_11;
			GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
			GPIO_InitStruct.Pull = GPIO_NOPULL;
			HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
			
			__HAL_AFIO_REMAP_USART3_PARTIAL();

		/* USER CODE BEGIN USART3_MspInit 1 */

		/* USER CODE END USART3_MspInit 1 */
		}
}

//TIM_HandleTypeDef htim3;
//TIM_HandleTypeDef htim4;

///**
//* @brief TIM_Base MSP Initialization
//* This function configures the hardware resources used in this example
//* @param htim_base: TIM_Base handle pointer
//* @retval None
//*/
//void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* htim_base)
//{
//  GPIO_InitTypeDef GPIO_InitStruct = {0};
//  if(htim_base->Instance==TIM1)
//  {
//  /* USER CODE BEGIN TIM1_MspInit 0 */

//  /* USER CODE END TIM1_MspInit 0 */
//    /* Peripheral clock enable */
//    __HAL_RCC_TIM1_CLK_ENABLE();
//  
//    __HAL_RCC_GPIOA_CLK_ENABLE();
//    /**TIM1 GPIO Configuration    
//    PA8     ------> TIM1_CH1 
//    */
//    GPIO_InitStruct.Pin = GPIO_PIN_8;
//    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
//    GPIO_InitStruct.Pull = GPIO_NOPULL;
//    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

//  /* USER CODE BEGIN TIM1_MspInit 1 */

//  /* USER CODE END TIM1_MspInit 1 */
//  }
//  else if(htim_base->Instance==TIM3)
//  {
//  /* USER CODE BEGIN TIM3_MspInit 0 */

//  /* USER CODE END TIM3_MspInit 0 */
//    /* Peripheral clock enable */
//    __HAL_RCC_TIM3_CLK_ENABLE();
//  /* USER CODE BEGIN TIM3_MspInit 1 */

//  /* USER CODE END TIM3_MspInit 1 */
//  }
//  else if(htim_base->Instance==TIM4)
//  {
//  /* USER CODE BEGIN TIM4_MspInit 0 */

//  /* USER CODE END TIM4_MspInit 0 */
//    /* Peripheral clock enable */
//    __HAL_RCC_TIM4_CLK_ENABLE();
//  /* USER CODE BEGIN TIM4_MspInit 1 */

//  /* USER CODE END TIM4_MspInit 1 */
//  }

//}

//void HAL_TIM_MspPostInit(TIM_HandleTypeDef* htim)
//{
//  GPIO_InitTypeDef GPIO_InitStruct = {0};
//  if(htim->Instance==TIM3)
//  {
//  /* USER CODE BEGIN TIM3_MspPostInit 0 */

//  /* USER CODE END TIM3_MspPostInit 0 */
//    __HAL_RCC_GPIOC_CLK_ENABLE();
//    /**TIM3 GPIO Configuration    
//    PC6     ------> TIM3_CH1
//    PC7     ------> TIM3_CH2
//    PC8     ------> TIM3_CH3
//    PC9     ------> TIM3_CH4 
//    */
//    GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9;
//    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
//    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
//    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

//    __HAL_AFIO_REMAP_TIM3_ENABLE();

//  /* USER CODE BEGIN TIM3_MspPostInit 1 */

//  /* USER CODE END TIM3_MspPostInit 1 */
//  }
//  else if(htim->Instance==TIM4)
//  {
//  /* USER CODE BEGIN TIM4_MspPostInit 0 */

//  /* USER CODE END TIM4_MspPostInit 0 */
//  
//    __HAL_RCC_GPIOB_CLK_ENABLE();
//    /**TIM4 GPIO Configuration    
//    PB6     ------> TIM4_CH1
//    PB7     ------> TIM4_CH2 
//    */
//    GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
//    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
//    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
//    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

//  /* USER CODE BEGIN TIM4_MspPostInit 1 */

//  /* USER CODE END TIM4_MspPostInit 1 */
//  }

//}

///**
//  * @brief TIM3 Initialization Function
//  * @param None
//  * @retval None
//  */
//static void MX_TIM3_Init(void)
//{

//  /* USER CODE BEGIN TIM3_Init 0 */
//	
//  /* USER CODE END TIM3_Init 0 */

//  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
//  TIM_MasterConfigTypeDef sMasterConfig = {0};
//  TIM_OC_InitTypeDef sConfigOC = {0};

//  /* USER CODE BEGIN TIM3_Init 1 */

//  /* USER CODE END TIM3_Init 1 */
//  htim3.Instance = TIM3;
//  htim3.Init.Prescaler = 0;
//  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
//  htim3.Init.Period = 3600;
//  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
//  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
//  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
//  {
//    Error_Handler();
//  }
//  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
//  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
//  {
//    Error_Handler();
//  }
//  if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
//  {
//    Error_Handler();
//  }
//  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
//  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
//  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
//  {
//    Error_Handler();
//  }
//  sConfigOC.OCMode = TIM_OCMODE_PWM1;
//  sConfigOC.Pulse = 1000;
//  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
//  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
//  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
//  {
//    Error_Handler();
//  }
//  sConfigOC.Pulse = 1800;
//  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
//  {
//    Error_Handler();
//  }
//  sConfigOC.Pulse = 2400;
//  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
//  {
//    Error_Handler();
//  }
//  sConfigOC.Pulse = 3200;
//  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
//  {
//    Error_Handler();
//  }
//  /* USER CODE BEGIN TIM3_Init 2 */
////	HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_1);
////	HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_2);
////	HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_3);
////	HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_4);
////	HAL_TIM_Base_Start(&htim3);
//  /* USER CODE END TIM3_Init 2 */
//  HAL_TIM_MspPostInit(&htim3);

//}

///**
//  * @brief TIM4 Initialization Function
//  * @param None
//  * @retval None
//  */
//static void MX_TIM4_Init(void)
//{

//  /* USER CODE BEGIN TIM4_Init 0 */

//  /* USER CODE END TIM4_Init 0 */

//  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
//  TIM_MasterConfigTypeDef sMasterConfig = {0};
//  TIM_OC_InitTypeDef sConfigOC = {0};

//  /* USER CODE BEGIN TIM4_Init 1 */

//  /* USER CODE END TIM4_Init 1 */
//  htim4.Instance = TIM4;
//  htim4.Init.Prescaler = 0;
//  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
//  htim4.Init.Period = 3600;
//  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
//  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
//  if (HAL_TIM_Base_Init(&htim4) != HAL_OK)
//  {
//    Error_Handler();
//  }
//  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
//  if (HAL_TIM_ConfigClockSource(&htim4, &sClockSourceConfig) != HAL_OK)
//  {
//    Error_Handler();
//  }
//  if (HAL_TIM_PWM_Init(&htim4) != HAL_OK)
//  {
//    Error_Handler();
//  }
//  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
//  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
//  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
//  {
//    Error_Handler();
//  }
//  sConfigOC.OCMode = TIM_OCMODE_PWM1;
//  sConfigOC.Pulse = 1800;
//  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
//  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
//  if (HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
//  {
//    Error_Handler();
//  }
//  if (HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
//  {
//    Error_Handler();
//  }
//  /* USER CODE BEGIN TIM4_Init 2 */
////	HAL_TIM_PWM_Start(&htim4,TIM_CHANNEL_1);
////	HAL_TIM_PWM_Start(&htim4,TIM_CHANNEL_2);
////	HAL_TIM_Base_Start(&htim4);
//  /* USER CODE END TIM4_Init 2 */
//  HAL_TIM_MspPostInit(&htim4);

//}
