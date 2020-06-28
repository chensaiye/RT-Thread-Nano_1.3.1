//#include "usart2_ctl.h" 
#include "usart2.h" 
#include <rtthread.h>
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
/* USER CODE END Includes */

//extern void MX_USART2_UART_Init(void);
extern uint8_t USART2_RX_BUF[USART2_REC_LEN];
extern uint16_t USART2_RX_STA;

#define USART2_THREAD_PRIORITY         8
#define USART2_THREAD_STACK_SIZE       512
#define USART2_THREAD_TIMESLICE        10

ALIGN(RT_ALIGN_SIZE)
static char thd_usart2_stack[USART2_THREAD_STACK_SIZE];
static struct rt_thread thd_usart2;


//void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
//{
//	if(huart->Instance==USART2)
//		USART2_RX_STA |= 0x8000;
////	if(huart->Instance==USART3)
////		USART3_RX_STA |= 0x8000;
//}

/**
  * @brief  The application entry point.
  * @retval int
  */
//void usart_ctl(void)
static void usart2_ctl_entey(void *parameter)
{
  /* USER CODE BEGIN 1 */
	char str[50]="test for uart2";
	USART2_RX_STA = 0;
	//MX_USART2_UART_Init();
	
	#ifdef USART2_TXSEND_WITH_DMA
		HAL_UART_Transmit_DMA(&huart2,(uint8_t *)str,12);//strlen(str));
	#else
		HAL_UART_Transmit(&huart2,(uint8_t *)str,strlen(str),10000);
	#endif
	
	HAL_UART_Receive_IT(&huart2,USART2_RX_BUF,USART2_REC_LEN);
	while (1)
  {
		if(USART2_RX_STA&0x8000)
		{
			#ifdef USART2_TXSEND_WITH_DMA
				HAL_UART_Transmit_DMA(&huart2,USART2_RX_BUF,USART2_RX_STA&0x7FFF);
			#else
			while(HAL_UART_Transmit(&huart2,USART2_RX_BUF,USART2_RX_STA&0x7FFF,10000)!=HAL_OK);
			#endif
			//
			USART2_RX_STA = 0;
			huart2.RxState = HAL_UART_STATE_READY;
			HAL_UART_Receive_IT(&huart2,USART2_RX_BUF,USART2_REC_LEN);
			
//			USART2_RX_STA = 0;
//			while(HAL_UART_Transmit(&huart2,USART2_RX_BUF,1,1000)!=HAL_OK);
//			HAL_UART_Receive_IT(&huart2,USART2_RX_BUF,1);
		}
		rt_thread_mdelay(10);
	}
}

int thread_usart2_init(void)
{
	//Button_IO_Init();
		rt_thread_init(&thd_usart2,
                   "thd_ut2",
                   usart2_ctl_entey,
                   RT_NULL,
                   &thd_usart2_stack[0],
                   sizeof(thd_usart2_stack),
                   USART2_THREAD_PRIORITY - 1, USART2_THREAD_TIMESLICE);
    rt_thread_startup(&thd_usart2);

    return 0;
}

MSH_CMD_EXPORT(thread_usart2_init, thread usart2 scan);

