#include "usart2.h" 
#include <rthw.h>
#include <rtthread.h>
extern void Error_Handler(void);

UART_HandleTypeDef huart2;

uint8_t USART2_RX_BUF[USART2_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
uint8_t USART2_TX_BUF[USART2_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
uint16_t USART2_RX_STA=0;       //接收状态标记	

#ifdef	USART2_RXOVER_WITH_TIM
TIM_HandleTypeDef htim6;
/**
  * @brief TIM6 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM6_Init(void)
{

  /* USER CODE BEGIN TIM6_Init 0 */

  /* USER CODE END TIM6_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM6_Init 1 */

  /* USER CODE END TIM6_Init 1 */
  htim6.Instance = TIM6;
  htim6.Init.Prescaler = 719;
  htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim6.Init.Period = 20*100;
  htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM6_Init 2 */
	//HAL_TIM_Base_Start_IT(&htim6);
  /* USER CODE END TIM6_Init 2 */

}

/**
  * @brief This function handles TIM6 global interrupt.
  */
void TIM6_IRQHandler(void)
{
  /* USER CODE BEGIN TIM6_IRQn 0 */

  /* USER CODE END TIM6_IRQn 0 */
  HAL_TIM_IRQHandler(&htim6);
  /* USER CODE BEGIN TIM6_IRQn 1 */
	
	//HAL_GPIO_TogglePin(GPIOA, GP1_POW_Pin);
	if((USART2_RX_STA&0x8000)==0)//接收未完成
	{
		USART2_RX_STA |= 0x8000;
		HAL_TIM_Base_Stop_IT(&htim6);
	}
  /* USER CODE END TIM6_IRQn 1 */
}

#endif


#ifdef USART2_TXSEND_WITH_DMA
DMA_HandleTypeDef hdma_usart2_tx;

/** 
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void) 
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel7_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel7_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel7_IRQn);

}

/**
  * @brief This function handles DMA1 channel7 global interrupt.
  */
void DMA1_Channel7_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Channel7_IRQn 0 */

  /* USER CODE END DMA1_Channel7_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_usart2_tx);
  /* USER CODE BEGIN DMA1_Channel7_IRQn 1 */

  /* USER CODE END DMA1_Channel7_IRQn 1 */
}

#endif

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */

static int MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */
	//必须先使能DMA ，再初始化UART
		#ifdef USART2_TXSEND_WITH_DMA
		MX_DMA_Init();
		#endif
  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */
		
	#ifdef	USART2_RXOVER_WITH_TIM
		MX_TIM6_Init();
	#endif
//	HAL_UART_Receive_IT(&huart2,USART2_RX_BUF,10);
//	HAL_UART_Receive(&huart2,USART2_RX_BUF,10,10000);
	
  /* USER CODE END USART2_Init 2 */
	return 0;
}
//INIT_BOARD_EXPORT(MX_USART2_UART_Init);

/**
* @brief UART MSP Initialization
* This function configures the hardware resources used in this example
* @param huart: UART handle pointer
* @retval None
*/
//void HAL_UART_MspInit(UART_HandleTypeDef* huart)
//{
//  GPIO_InitTypeDef GPIO_InitStruct = {0};
//  if(huart->Instance==USART2)
//  {
//  /* USER CODE BEGIN USART2_MspInit 0 */

//  /* USER CODE END USART2_MspInit 0 */
//    /* Peripheral clock enable */
//    __HAL_RCC_USART2_CLK_ENABLE();
//  
//    __HAL_RCC_GPIOA_CLK_ENABLE();
//    /**USART2 GPIO Configuration    
//    PA2     ------> USART2_TX
//    PA3     ------> USART2_RX 
//    */
//    GPIO_InitStruct.Pin = GPIO_PIN_2;
//    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
//    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
//    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

//    GPIO_InitStruct.Pin = GPIO_PIN_3;
//    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
//    GPIO_InitStruct.Pull = GPIO_NOPULL;
//    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

//		#ifdef USART2_TXSEND_WITH_DMA
//    /* USART2 DMA Init */
//    /* USART2_TX Init */
//    hdma_usart2_tx.Instance = DMA1_Channel7;
//    hdma_usart2_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
//    hdma_usart2_tx.Init.PeriphInc = DMA_PINC_DISABLE;
//    hdma_usart2_tx.Init.MemInc = DMA_MINC_ENABLE;
//    hdma_usart2_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
//    hdma_usart2_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
//    hdma_usart2_tx.Init.Mode = DMA_NORMAL;
//    hdma_usart2_tx.Init.Priority = DMA_PRIORITY_LOW;
//    if (HAL_DMA_Init(&hdma_usart2_tx) != HAL_OK)
//    {
//      Error_Handler();
//    }

//    __HAL_LINKDMA(huart,hdmatx,hdma_usart2_tx);
//		#endif
//		
//    /* USART2 interrupt Init */
//    HAL_NVIC_SetPriority(USART2_IRQn, 3, 2);
//    HAL_NVIC_EnableIRQ(USART2_IRQn);
//  /* USER CODE BEGIN USART2_MspInit 1 */

//  /* USER CODE END USART2_MspInit 1 */
//  }
//  
//}



/**
  * @brief This function handles USART2 global interrupt.
  */
void USART2_IRQHandler(void)
{
  /* USER CODE BEGIN USART2_IRQn 0 */
	#ifdef USART2_RXOVER_WITH_TIM
	if((huart2.Instance->SR & USART_SR_RXNE) != RESET)
	{
		if((USART2_RX_STA&0x8000)==0)//接收未完成
		{
			USART2_RX_STA++;
			__HAL_TIM_SET_COUNTER(&htim6,0);
			HAL_TIM_Base_Start_IT(&htim6);
		}
	}
	#endif
  /* USER CODE END USART2_IRQn 0 */
  HAL_UART_IRQHandler(&huart2);
  /* USER CODE BEGIN USART2_IRQn 1 */
	
  /* USER CODE END USART2_IRQn 1 */
}







