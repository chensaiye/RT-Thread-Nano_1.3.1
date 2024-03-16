#include "iic_bus.h"

I2C_HandleTypeDef hi2c1;
I2C_HandleTypeDef hi2c2;

extern void Error_Handler(void);

/**
  * @brief  Initialize the I2C MSP.
  * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
  *         the configuration information for the specified I2C.
  * @retval None
  */
void HAL_I2C_MspInit(I2C_HandleTypeDef *hi2c)
{
  
	GPIO_InitTypeDef GPIO_InitStruct = {0};
//  if(hi2c->Instance==I2C1)
//  {
//  /* USER CODE BEGIN I2C1_MspInit 0 */

//  /* USER CODE END I2C1_MspInit 0 */
//  
//    __HAL_RCC_GPIOB_CLK_ENABLE();
//    /**I2C1 GPIO Configuration    
//    PB8     ------> I2C1_SCL
//    PB9     ------> I2C1_SDA 
//    */
//    GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9;
//    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
//    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
//    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

//    __HAL_AFIO_REMAP_I2C1_ENABLE();

//    /* Peripheral clock enable */
//    __HAL_RCC_I2C1_CLK_ENABLE();
//  /* USER CODE BEGIN I2C1_MspInit 1 */

//  /* USER CODE END I2C1_MspInit 1 */
//  }
//  else 
	if(hi2c->Instance==I2C2)
  {
  /* USER CODE BEGIN I2C2_MspInit 0 */

  /* USER CODE END I2C2_MspInit 0 */
  
    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**I2C2 GPIO Configuration    
    PB10     ------> I2C2_SCL
    PB11     ------> I2C2_SDA 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* Peripheral clock enable */
    __HAL_RCC_I2C2_CLK_ENABLE();
  /* USER CODE BEGIN I2C2_MspInit 1 */

  /* USER CODE END I2C2_MspInit 1 */
  }
  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_I2C_MspInit could be implemented in the user file
   */
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	__HAL_RCC_GPIOB_CLK_ENABLE();
	GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9;/*Configure GPIO pins : PB8 PB9 */
  GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
   // Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief I2C2 Initialization Function
  * @param None
  * @retval None
  */
void MX_I2C2_Init(void)
{

  /* USER CODE BEGIN I2C2_Init 0 */
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_I2C2_CLK_ENABLE();
  /* USER CODE END I2C2_Init 0 */

  /* USER CODE BEGIN I2C2_Init 1 */
	
  /* USER CODE END I2C2_Init 1 */
	
  hi2c2.Instance = I2C2;
  hi2c2.Init.ClockSpeed = 400000;
  hi2c2.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c2.Init.OwnAddress1 = 0x55;
  hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c2.Init.OwnAddress2 = 0xa0;
  hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C2_Init 2 */

  /* USER CODE END I2C2_Init 2 */

}


////I2C_Master_BufferWrite I2C主机写数据
////I2Cx  					:使用的IIC接口
////pBuffer					:要写入的数据
////NumByteToWrite  :写入数据长度
////SlaveAddress 		: 从机设备地址
////WriteAddress 		: 要写入的数据地址
//uint8_t I2C_Master_BufferWrite(I2C_TypeDef * I2Cx, uint8_t* pBuffer, uint32_t NumByteToWrite, uint8_t SlaveAddress, uint8_t WriteAddress)
//{
////		uint32_t tmr;
////		tmr = ulTimeOut_Time;
//   
//    if(NumByteToWrite==0)
//        return 255;
//		HAL_I2C_Master_Transmit(&hi2c1,SlaveAddress,pBuffer,NumByteToWrite,ulTimeOut_Time);
////    /* 1.开始*/
////    I2C_GenerateSTART(I2Cx, ENABLE);
////    while((--tmr)&&(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT)));
////		if(tmr==0) return 1;
////		
////    /* 2.设备地址·/写 */
////		tmr = ulTimeOut_Time;
////    I2C_Send7bitAddress(I2Cx, SlaveAddress, I2C_Direction_Transmitter);
////    while((--tmr)&&(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)));
////		if(tmr==0) return 2;
////		
////		 /* 3.发送操作的内存地址*/
////		tmr = ulTimeOut_Time;
////		I2C_SendData(I2Cx,WriteAddress);
////		while((--tmr)&&(!I2C_CheckEvent(I2Cx,I2C_EVENT_MASTER_BYTE_TRANSMITTED)));
////		if(tmr==0) return 3;
////		
////    /* 4.连续写数据 */
////    while(NumByteToWrite--)
////    {
////			tmr = ulTimeOut_Time;
////			I2C_SendData(I2Cx, *pBuffer);
////      while((--tmr)&&(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED)));
////			if(tmr==0) return 4;
////      pBuffer++;
////		}

////    /* 5.停止 */
////		tmr = ulTimeOut_Time;
////    I2C_GenerateSTOP(I2Cx, ENABLE);
////    while ((--tmr)&&((I2Cx->CR1&0x200) == 0x200));
////		if(tmr==0) return 5;
//    return 0;
//}
////I2C_Master_BufferRead  I2C主机读数据
////I2Cx  					:使用的IIC接口
////pBuffer					:读取数据的缓冲区地址
////NumByteToWrite  :读取的数据长度
////SlaveAddress 		:从机设备地址
////ReadAddress 		:要读取的起始地址
//uint8_t I2C_Master_BufferRead(I2C_TypeDef * I2Cx, uint8_t* pBuffer, uint32_t NumByteToRead, uint8_t SlaveAddress, uint8_t ReadAddress)
//{
////		uint32_t tmr;
////		tmr = ulTimeOut_Time;
//	
//    if(NumByteToRead==0)
//        return 255;
//		HAL_I2C_Master_Receive(&hi2c1,SlaveAddress,pBuffer,NumByteToRead,ulTimeOut_Time);
////		 /* 254.总线忙超时*/
////    while((--tmr)&&(I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY))); 
////		if(tmr==0) return 254;	
////		
////    I2C_AcknowledgeConfig(I2Cx, ENABLE);
////    /* 1.开始*/
////		tmr = ulTimeOut_Time;
////    I2C_GenerateSTART(I2Cx, ENABLE);
////    while((--tmr)&&(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT)));
////		if(tmr==0) return 1;
////		
////    /* 2.设备地址·/写 */
////		tmr = ulTimeOut_Time;
////    I2C_Send7bitAddress(I2Cx, SlaveAddress, I2C_Direction_Transmitter);
////    while ((--tmr)&&(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)));
////		if(tmr==0) return 2;
////		
////		 /* 3.发送操作的内存地址*/
////		tmr = ulTimeOut_Time;
////		I2C_SendData(I2C1,ReadAddress);
////		while((--tmr)&&(!I2C_CheckEvent(I2Cx,I2C_EVENT_MASTER_BYTE_TRANSMITTED)));
////		if(tmr==0) return 3;
////		
////    /* 4.开始*/
////		tmr = ulTimeOut_Time;
////    I2C_GenerateSTART(I2Cx, ENABLE);
////    while((--tmr)&&(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT)));
////		if(tmr==0) return 4;
////		
////    /* 5.设备地址·/读 */
////		tmr = ulTimeOut_Time;
////    I2C_Send7bitAddress(I2Cx, SlaveAddress, I2C_Direction_Receiver);
////    while((--tmr)&&(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)));
////		if(tmr==0) return 5;
////		
////    /* 6.连续写数据 */
////    while (NumByteToRead)
////    {
////        if(NumByteToRead==1)
////        {
////            I2C_AcknowledgeConfig(I2Cx, DISABLE);
////            I2C_GenerateSTOP(I2Cx, ENABLE);//6.停止，非应答
////        }
////				tmr = ulTimeOut_Time;
////        while((--tmr)&&(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED)));  /* EV7 */
////				if(tmr==0) return 6;
////        *pBuffer++ = I2C_ReceiveData(I2Cx);
////        NumByteToRead--;
////    }

////    I2C_AcknowledgeConfig(I2Cx, ENABLE);
//    return 0;
//}   

