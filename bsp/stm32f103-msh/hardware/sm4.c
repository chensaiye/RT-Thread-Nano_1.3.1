#include "sm4.h"
/********************************************************************************/
//////////////////////////////////////////////////////////////////////////////////	
//4λ���뿪������ ��������
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//STM32������
//CSY
//�޸�����:2020/4/16
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) 
//All rights reserved									   
//////////////////////////////////////////////////////////////////////////////////
/********************************************************************************/
//�޸�˵�� 
//V1.0 Ϊ��ӦRT-Thread V3.1.3
////////////////////////////////////////////////////////////////////////////////// 
/********************************************************************************/
#include "stm32f1xx_hal.h"
#include <rthw.h>
#include <rtthread.h>

//��ʼ����ʹ��ʱ��		    
int SM4_Init(void)
{
		GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOC_CLK_ENABLE();
		GPIO_InitStruct.Pin   = SM_Pin1 | SM_Pin2 | SM_Pin3| SM_Pin4;
    GPIO_InitStruct.Mode  = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
		return 0;
}
//INIT_BOARD_EXPORT(SM4_Init);

uint8_t SM4_Get_Date(void)
{
	 uint8_t i=0xFF;
	 i = HAL_GPIO_ReadPin(SM_GPIO_Port,SM_Pin1) | (HAL_GPIO_ReadPin(SM_GPIO_Port,SM_Pin2)<<1) | \
			(HAL_GPIO_ReadPin(SM_GPIO_Port,SM_Pin3)<<2) |(HAL_GPIO_ReadPin(SM_GPIO_Port,SM_Pin4)<<3);
	 return (i&0xFF);
}



