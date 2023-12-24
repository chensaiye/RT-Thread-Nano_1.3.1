#include "usart3.h" 
#include <rthw.h>
#include <rtthread.h>
extern void Error_Handler(void);

UART_HandleTypeDef huart3;

uint8_t USART3_RX_BUF[USART3_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
uint8_t USART3_TX_BUF[USART3_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
uint16_t USART3_RX_STA=0;       //����״̬���

#ifdef	USART3_RXOVER_WITH_TIM
TIM_HandleTypeDef htim7;

/**
  * @brief TIM7 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM7_Init(void)
{

  /* USER CODE BEGIN TIM7_Init 0 */

  /* USER CODE END TIM7_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM7_Init 1 */

  /* USER CODE END TIM7_Init 1 */
  htim7.Instance = TIM7;
  htim7.Init.Prescaler = 719;
  htim7.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim7.Init.Period = 3500000/9600;//accord to the BaudRate set the overtime,  72M/720*(10*3.5)/BaudRate=
  htim7.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim7) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim7, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM7_Init 2 */

  /* USER CODE END TIM7_Init 2 */

}

/**
  * @brief This function handles TIM7 global interrupt.
  */
void TIM7_IRQHandler(void)
{
  /* USER CODE BEGIN TIM7_IRQn 0 */

  /* USER CODE END TIM7_IRQn 0 */
  HAL_TIM_IRQHandler(&htim7);
  /* USER CODE BEGIN TIM7_IRQn 1 */
	if((USART3_RX_STA&0x8000)==0)//����δ���
	{
		USART3_RX_STA |= 0x8000;
		HAL_TIM_Base_Stop_IT(&htim7);
	}
  /* USER CODE END TIM7_IRQn 1 */
}


#endif

#ifdef USART3_TXSEND_WITH_DMA
DMA_HandleTypeDef hdma_usart3_tx;

/** 
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void) 
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel2_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel2_IRQn);

}

/**
  * @brief This function handles DMA1 channel2 global interrupt.
  */
void DMA1_Channel2_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Channel2_IRQn 0 */

  /* USER CODE END DMA1_Channel2_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_usart3_tx);
  /* USER CODE BEGIN DMA1_Channel2_IRQn 1 */

  /* USER CODE END DMA1_Channel2_IRQn 1 */
}

#endif


/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static int MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */
		//������ʹ��DMA ���ٳ�ʼ��UART
		#ifdef USART3_TXSEND_WITH_DMA
		MX_DMA_Init();
		#endif
  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 9600;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */
	#ifdef	USART3_RXOVER_WITH_TIM
		MX_TIM7_Init();
	#endif
	
	return 0;
  /* USER CODE END USART3_Init 2 */

}
INIT_BOARD_EXPORT(MX_USART3_UART_Init);

/**
  * @brief This function handles USART2 global interrupt.
  */
void USART3_IRQHandler(void)
{
  /* USER CODE BEGIN USART2_IRQn 0 */
	#ifdef USART3_RXOVER_WITH_TIM
	if((huart3.Instance->SR & USART_SR_RXNE) != RESET)
	{
		if((USART3_RX_STA&0x8000)==0)//����δ���
		{
			USART3_RX_STA++;
			__HAL_TIM_SET_COUNTER(&htim7,0);
			HAL_TIM_Base_Start_IT(&htim7);
		}
	}
	#endif
  /* USER CODE END USART2_IRQn 0 */
  HAL_UART_IRQHandler(&huart3);
  /* USER CODE BEGIN USART2_IRQn 1 */
	
  /* USER CODE END USART2_IRQn 1 */
}


