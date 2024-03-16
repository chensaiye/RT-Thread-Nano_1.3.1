#include "usart3.h" 
#include <rtthread.h>
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
/* USER CODE END Includes */
#include "main_control.h"
//extern void MX_USART2_UART_Init(void);
extern uint8_t USART3_RX_BUF[USART3_REC_LEN];
extern uint16_t USART3_RX_STA;

#define USART3_THREAD_PRIORITY         8
#define USART3_THREAD_STACK_SIZE       512
#define USART3_THREAD_TIMESLICE        10

ALIGN(RT_ALIGN_SIZE)
static char thd_usart3_stack[USART3_THREAD_STACK_SIZE];
static struct rt_thread thd_usart3;


//void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
//{
////	if(huart->Instance==USART2)
////		USART2_RX_STA |= 0x8000;
//	if(huart->Instance==USART3)
//		USART3_RX_STA |= 0x8000;
//}

/**
  * @brief  The application entry point.
  * @retval int
  */
//void usart_ctl(void)
static void usart3_ctl_entey(void *parameter)
{
  /* USER CODE BEGIN 1 */
	//char str[50]="test for uart3";
	USART3_RX_STA = 0;
	//MX_USART3_UART_Init();
	
	// #ifdef USART3_TXSEND_WITH_DMA
	// 	HAL_UART_Transmit_DMA(&huart3,(uint8_t *)str,strlen(str));
	// #else
	// 	HAL_UART_Transmit(&huart3,(uint8_t *)str,strlen(str),10000);
	// #endif
	
	HAL_UART_Receive_IT(&huart3,USART3_RX_BUF,USART3_REC_LEN);
	while (1)
  {
		#if 0	//RX TX test
		if(USART3_RX_STA&0x8000)
		{
			#ifdef USART3_TXSEND_WITH_DMA
				HAL_UART_Transmit_DMA(&huart3,USART3_RX_BUF,USART3_RX_STA&0x7FFF);
			#else
			while(HAL_UART_Transmit(&huart3,USART3_RX_BUF,USART3_RX_STA&0x7FFF,10000)!=HAL_OK);
			#endif
			//
			USART3_RX_STA = 0;
			huart3.RxState = HAL_UART_STATE_READY;
			HAL_UART_Receive_IT(&huart3,USART3_RX_BUF,USART3_REC_LEN);
			
		}
		#endif
		#if 1 // add logic
		if(USART3_RX_STA&0x8000)
		{
			if(remote_bag_in(USART3_RX_BUF)==1)
			{
				#ifdef USART3_TXSEND_WITH_DMA
					HAL_UART_Transmit_DMA(&huart3,USART3_RX_BUF,BAG_LENGTH);
				#else
					HAL_UART_Transmit(&huart3,USART3_RX_BUF,BAG_LENGTH,10000);
				#endif
			}
			USART3_RX_STA = 0;
			huart3.RxState = HAL_UART_STATE_READY;
			HAL_UART_Receive_IT(&huart3,USART3_RX_BUF,USART3_REC_LEN);
		}
		#endif
		rt_thread_mdelay(10);
	}
}

int thread_usart3_init(void)
{
	//Button_IO_Init();
		rt_thread_init(&thd_usart3,
                   "thd_ut3",
                   usart3_ctl_entey,
                   RT_NULL,
                   &thd_usart3_stack[0],
                   sizeof(thd_usart3_stack),
                   USART3_THREAD_PRIORITY - 1, USART3_THREAD_TIMESLICE);
    rt_thread_startup(&thd_usart3);

    return 0;
}

MSH_CMD_EXPORT(thread_usart3_init, thread usart3 scan);

