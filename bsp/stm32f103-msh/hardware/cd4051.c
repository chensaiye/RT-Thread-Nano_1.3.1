#include "cd4051.h"
#include "stm32f1xx_hal.h"
#include <rthw.h>
#include <rtthread.h>
/********************************************************************************/
//////////////////////////////////////////////////////////////////////////////////	
//LED ��������
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//STM32������
//CSY
//�޸�����:2015/10/09
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ����Ӧ��ҽ����е���޹�˾
//All rights reserved									   
//////////////////////////////////////////////////////////////////////////////////
/********************************************************************************/
//�޸�˵�� 

////////////////////////////////////////////////////////////////////////////////// 
/********************************************************************************/


//cd4051 IO��ʼ��
void CD4051_Init(void)
{
   GPIO_InitTypeDef  GPIO_InitStructure;
	//cd4051�˿�����
	 __HAL_RCC_GPIOB_CLK_ENABLE();
	  GPIO_InitStructure.Pin   = CD4051_A |CD4051_B |CD4051_C;
    GPIO_InitStructure.Mode  = GPIO_MODE_OUTPUT_PP;//�������
    GPIO_InitStructure.Pull  = GPIO_NOPULL;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_LOW;
	 HAL_GPIO_Init(CD4051_Port, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOC
	 
}
INIT_BOARD_EXPORT(CD4051_Init);

//4051��� ,chͨ�� ��endis
//void CD4051_Ch_Set(uint8_t ch, uint8_t endis)
//{
//	uint16_t tp;
//	ch &= 0x07; //0~7ֵ
//	if(endis)
//	{
//		tp = GPIO_ReadOutputData(GPIOB);
//		tp &= 0x1FFF;
//		tp = (ch<<13) | tp;
//		GPIO_Write(GPIOB,tp);
//	}	
//}

//void CD4051_Ch_Select(uint16_t ch)
//{
//	uint16_t tp;
//	ch &= 0x07; //0~7ֵ
//	ch <<= 13;
//		
//		tp = GPIO_ReadOutputData(GPIOB);
//		tp &= 0xFFF8;
//		tp = ch | tp;
//		GPIO_Write(GPIOB,tp);	
//}

void CD4051_Ch_Select(uint16_t ch)
{
	ch &= 0x07; //0~7ֵ
	if((ch & 0x01)==0)
		HAL_GPIO_WritePin(CD4051_Port,CD4051_A,GPIO_PIN_RESET);
	else
		HAL_GPIO_WritePin(CD4051_Port,CD4051_A,GPIO_PIN_SET);
	
	if((ch & 0x02)==0)
		HAL_GPIO_WritePin(CD4051_Port,CD4051_B,GPIO_PIN_RESET);
	else
		HAL_GPIO_WritePin(CD4051_Port,CD4051_B,GPIO_PIN_SET);
	
	if((ch & 0x04)==0)
		HAL_GPIO_WritePin(CD4051_Port,CD4051_C,GPIO_PIN_RESET);
	else
		HAL_GPIO_WritePin(CD4051_Port,CD4051_C,GPIO_PIN_SET);
}
