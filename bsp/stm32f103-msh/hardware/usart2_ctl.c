//#include "usart2_ctl.h" 
#include "usart2.h" 
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
/* USER CODE END Includes */

extern void MX_USART2_UART_Init(void);
extern uint8_t USART2_RX_BUF[];
extern uint16_t USART2_RX_STA;


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	USART2_RX_STA |= 0x8000;
}

/**
  * @brief  The application entry point.
  * @retval int
  */
void usart_ctl(void)
{
  /* USER CODE BEGIN 1 */
	char str[50]="test for uart2";
	MX_USART2_UART_Init();
	
	//HAL_UART_Transmit(&huart2,(uint8_t *)str,strlen(str),10000);
	HAL_UART_Transmit_DMA(&huart2,(uint8_t *)str,strlen(str));
	
	HAL_UART_Receive_IT(&huart2,USART2_RX_BUF,USART2_REC_LEN);
	while (1)
  {
		if(USART2_RX_STA&0x8000)
		{
			//while(HAL_UART_Transmit(&huart2,USART2_RX_BUF,USART2_RX_STA&0x7FFF,10000)!=HAL_OK);
			HAL_UART_Transmit_DMA(&huart2,USART2_RX_BUF,USART2_RX_STA&0x7FFF);
			USART2_RX_STA = 0;
			huart2.RxState = HAL_UART_STATE_READY;
			HAL_UART_Receive_IT(&huart2,USART2_RX_BUF,USART2_REC_LEN);
			
//			USART2_RX_STA = 0;
//			while(HAL_UART_Transmit(&huart2,USART2_RX_BUF,1,1000)!=HAL_OK);
//			HAL_UART_Receive_IT(&huart2,USART2_RX_BUF,1);
			
		}
	}
	
}

