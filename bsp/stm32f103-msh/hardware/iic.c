#include "iic.h"

I2C_HandleTypeDef hi2c1;

#define ulTimeOut_Time 10000

//void I2C1_GPIO_Configuration(void)
//{
//  GPIO_InitTypeDef  GPIO_InitStructure;

//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;//必须设置为开漏输出，实现iic的线与逻辑
//    //GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//    //GPIO_InitStructure.GPIO_PuPd =   GPIO_PuPd_NOPULL;
//    GPIO_Init(GPIOB, &GPIO_InitStructure);
//		GPIO_PinRemapConfig(GPIO_Remap_I2C1,ENABLE);
////    GPIO_PinAFConfig(GPIOB,GPIO_PinSource8,GPIO_AF_I2C1); 
////    GPIO_PinAFConfig(GPIOB,GPIO_PinSource9,GPIO_AF_I2C1);
//}
//void I2C1_Configuration(void)
//{
//    I2C_InitTypeDef I2C_InitStructure;

//    I2C_DeInit(I2C1);
//    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
//    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
//    I2C_InitStructure.I2C_OwnAddress1 = 0X55;//主机的地址        
//    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
//    I2C_InitStructure.I2C_AcknowledgedAddress= I2C_AcknowledgedAddress_7bit;
//    I2C_InitStructure.I2C_ClockSpeed = 100000;//100KHZ
//    I2C_Init(I2C1, &I2C_InitStructure);
//    I2C_Cmd(I2C1, ENABLE);                                             
//}

//void I2C1_Init(void)
//{
//    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
//    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);   
//    I2C1_GPIO_Configuration();
//    I2C1_Configuration();
//}



/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
//static void MX_I2C1_Init(void)
//{

//  /* USER CODE BEGIN I2C1_Init 0 */
//	GPIO_InitTypeDef GPIO_InitStruct = {0};
//	__HAL_RCC_GPIOB_CLK_ENABLE();
//	GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9;/*Configure GPIO pins : PB8 PB9 */
//  GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
//  GPIO_InitStruct.Pull = GPIO_NOPULL;
//  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
//  /* USER CODE END I2C1_Init 0 */

//  /* USER CODE BEGIN I2C1_Init 1 */

//  /* USER CODE END I2C1_Init 1 */
//  hi2c1.Instance = I2C1;
//  hi2c1.Init.ClockSpeed = 100000;
//  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
//  hi2c1.Init.OwnAddress1 = 0;
//  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
//  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
//  hi2c1.Init.OwnAddress2 = 0;
//  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
//  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
//  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
//  {
//   // Error_Handler();
//  }
//  /* USER CODE BEGIN I2C1_Init 2 */

//  /* USER CODE END I2C1_Init 2 */

//}



//void I2C2_GPIO_Configuration(void)
//{
//  GPIO_InitTypeDef  GPIO_InitStructure;

//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
//    //GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;//必须设置为开漏输出，实现iic的线与逻辑
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//    //GPIO_InitStructure.GPIO_PuPd =   GPIO_PuPd_NOPULL;
//    GPIO_Init(GPIOB, &GPIO_InitStructure);
////    GPIO_PinAFConfig(GPIOB,GPIO_PinSource10,GPIO_AF_I2C2); 
////    GPIO_PinAFConfig(GPIOB,GPIO_PinSource11,GPIO_AF_I2C2);
//}
//void I2C2_Configuration(void)
//{
//    I2C_InitTypeDef I2C_InitStructure;

//    I2C_DeInit(I2C2);
//    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
//    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
//    I2C_InitStructure.I2C_OwnAddress1 = 0X55;//主机的地址        
//    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
//    I2C_InitStructure.I2C_AcknowledgedAddress= I2C_AcknowledgedAddress_7bit;
//    I2C_InitStructure.I2C_ClockSpeed = 100000;//100KHZ
//    I2C_Init(I2C2, &I2C_InitStructure);
//    I2C_Cmd(I2C2, ENABLE);                                             
//}
//void I2C2_Init(void)
//{
//    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);
//    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);   
//    I2C2_GPIO_Configuration();
//    I2C2_Configuration();
//}

//I2C_Master_BufferWrite I2C主机写数据
//I2Cx  					:使用的IIC接口
//pBuffer					:要写入的数据
//NumByteToWrite  :写入数据长度
//SlaveAddress 		: 从机设备地址
//WriteAddress 		: 要写入的数据地址
uint8_t I2C_Master_BufferWrite(I2C_TypeDef * I2Cx, uint8_t* pBuffer, uint32_t NumByteToWrite, uint8_t SlaveAddress, uint8_t WriteAddress)
{
//		uint32_t tmr;
//		tmr = ulTimeOut_Time;
   
    if(NumByteToWrite==0)
        return 255;
		HAL_I2C_Master_Transmit(&hi2c1,SlaveAddress,pBuffer,NumByteToWrite,ulTimeOut_Time);
//    /* 1.开始*/
//    I2C_GenerateSTART(I2Cx, ENABLE);
//    while((--tmr)&&(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT)));
//		if(tmr==0) return 1;
//		
//    /* 2.设备地址·/写 */
//		tmr = ulTimeOut_Time;
//    I2C_Send7bitAddress(I2Cx, SlaveAddress, I2C_Direction_Transmitter);
//    while((--tmr)&&(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)));
//		if(tmr==0) return 2;
//		
//		 /* 3.发送操作的内存地址*/
//		tmr = ulTimeOut_Time;
//		I2C_SendData(I2Cx,WriteAddress);
//		while((--tmr)&&(!I2C_CheckEvent(I2Cx,I2C_EVENT_MASTER_BYTE_TRANSMITTED)));
//		if(tmr==0) return 3;
//		
//    /* 4.连续写数据 */
//    while(NumByteToWrite--)
//    {
//			tmr = ulTimeOut_Time;
//			I2C_SendData(I2Cx, *pBuffer);
//      while((--tmr)&&(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED)));
//			if(tmr==0) return 4;
//      pBuffer++;
//		}

//    /* 5.停止 */
//		tmr = ulTimeOut_Time;
//    I2C_GenerateSTOP(I2Cx, ENABLE);
//    while ((--tmr)&&((I2Cx->CR1&0x200) == 0x200));
//		if(tmr==0) return 5;
    return 0;
}
//I2C_Master_BufferRead  I2C主机读数据
//I2Cx  					:使用的IIC接口
//pBuffer					:读取数据的缓冲区地址
//NumByteToWrite  :读取的数据长度
//SlaveAddress 		:从机设备地址
//ReadAddress 		:要读取的起始地址
uint8_t I2C_Master_BufferRead(I2C_TypeDef * I2Cx, uint8_t* pBuffer, uint32_t NumByteToRead, uint8_t SlaveAddress, uint8_t ReadAddress)
{
//		uint32_t tmr;
//		tmr = ulTimeOut_Time;
	
    if(NumByteToRead==0)
        return 255;
		HAL_I2C_Master_Receive(&hi2c1,SlaveAddress,pBuffer,NumByteToRead,ulTimeOut_Time);
//		 /* 254.总线忙超时*/
//    while((--tmr)&&(I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY))); 
//		if(tmr==0) return 254;	
//		
//    I2C_AcknowledgeConfig(I2Cx, ENABLE);
//    /* 1.开始*/
//		tmr = ulTimeOut_Time;
//    I2C_GenerateSTART(I2Cx, ENABLE);
//    while((--tmr)&&(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT)));
//		if(tmr==0) return 1;
//		
//    /* 2.设备地址·/写 */
//		tmr = ulTimeOut_Time;
//    I2C_Send7bitAddress(I2Cx, SlaveAddress, I2C_Direction_Transmitter);
//    while ((--tmr)&&(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)));
//		if(tmr==0) return 2;
//		
//		 /* 3.发送操作的内存地址*/
//		tmr = ulTimeOut_Time;
//		I2C_SendData(I2C1,ReadAddress);
//		while((--tmr)&&(!I2C_CheckEvent(I2Cx,I2C_EVENT_MASTER_BYTE_TRANSMITTED)));
//		if(tmr==0) return 3;
//		
//    /* 4.开始*/
//		tmr = ulTimeOut_Time;
//    I2C_GenerateSTART(I2Cx, ENABLE);
//    while((--tmr)&&(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT)));
//		if(tmr==0) return 4;
//		
//    /* 5.设备地址·/读 */
//		tmr = ulTimeOut_Time;
//    I2C_Send7bitAddress(I2Cx, SlaveAddress, I2C_Direction_Receiver);
//    while((--tmr)&&(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)));
//		if(tmr==0) return 5;
//		
//    /* 6.连续写数据 */
//    while (NumByteToRead)
//    {
//        if(NumByteToRead==1)
//        {
//            I2C_AcknowledgeConfig(I2Cx, DISABLE);
//            I2C_GenerateSTOP(I2Cx, ENABLE);//6.停止，非应答
//        }
//				tmr = ulTimeOut_Time;
//        while((--tmr)&&(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED)));  /* EV7 */
//				if(tmr==0) return 6;
//        *pBuffer++ = I2C_ReceiveData(I2Cx);
//        NumByteToRead--;
//    }

//    I2C_AcknowledgeConfig(I2Cx, ENABLE);
    return 0;
}   

