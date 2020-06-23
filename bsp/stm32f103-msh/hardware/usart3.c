#include "usart3.h" 

extern void Error_Handler(void);

UART_HandleTypeDef huart3;


/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
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

  /* USER CODE END USART3_Init 2 */

}

/**
* @brief UART MSP Initialization
* This function configures the hardware resources used in this example
* @param huart: UART handle pointer
* @retval None
*/
//void HAL_UART_MspInit(UART_HandleTypeDef* huart)
//{
//  GPIO_InitTypeDef GPIO_InitStruct = {0};
//  if(huart->Instance==USART3)
//  {
//  /* USER CODE BEGIN USART3_MspInit 0 */

//  /* USER CODE END USART3_MspInit 0 */
//    /* Peripheral clock enable */
//    __HAL_RCC_USART3_CLK_ENABLE();
//  
//    __HAL_RCC_GPIOC_CLK_ENABLE();
//    /**USART3 GPIO Configuration    
//    PC10     ------> USART3_TX
//    PC11     ------> USART3_RX 
//    */
//    GPIO_InitStruct.Pin = GPIO_PIN_10;
//    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
//    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
//    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

//    GPIO_InitStruct.Pin = GPIO_PIN_11;
//    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
//    GPIO_InitStruct.Pull = GPIO_NOPULL;
//    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

//    __HAL_AFIO_REMAP_USART3_PARTIAL();

//  /* USER CODE BEGIN USART3_MspInit 1 */

//  /* USER CODE END USART3_MspInit 1 */
//  }

//}

/**
* @brief UART MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param huart: UART handle pointer
* @retval None
*/
void HAL_UART_MspDeInit(UART_HandleTypeDef* huart)
{
  if(huart->Instance==USART3)
  {
  /* USER CODE BEGIN USART3_MspDeInit 0 */

  /* USER CODE END USART3_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART3_CLK_DISABLE();
  
    /**USART3 GPIO Configuration    
    PC10     ------> USART3_TX
    PC11     ------> USART3_RX 
    */
    HAL_GPIO_DeInit(GPIOC, GPIO_PIN_10|GPIO_PIN_11);

  /* USER CODE BEGIN USART3_MspDeInit 1 */

  /* USER CODE END USART3_MspDeInit 1 */
  }

}







