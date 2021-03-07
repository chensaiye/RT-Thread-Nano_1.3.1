#ifndef __MYIIC_H
#define __MYIIC_H

#include "stm32f1xx_hal.h"
#include <rtthread.h>
#include <rtdef.h>
#include <rthw.h>

#define I2C_SCL_PIN          		GPIO_PIN_8
#define I2C_SCL_PORT    				GPIOB
#define I2C_SCL_CLK       			RCC_APB2Periph_GPIOB 

#define I2C_SDA_PIN             GPIO_PIN_9
#define I2C_SDA_PORT          	GPIOB
#define I2C_SDA_CLK           	RCC_APB2Periph_GPIOB
	 
//IO��������	
#define USERI2C_SCL_1   	HAL_GPIO_WritePin(I2C_SCL_PORT, I2C_SCL_PIN,GPIO_PIN_SET)
#define USERI2C_SCL_0   	HAL_GPIO_WritePin(I2C_SCL_PORT, I2C_SCL_PIN,GPIO_PIN_RESET)
#define USERI2C_SDA_1   	HAL_GPIO_WritePin(I2C_SDA_PORT, I2C_SDA_PIN,GPIO_PIN_SET)
#define USERI2C_SDA_0   	HAL_GPIO_WritePin(I2C_SDA_PORT, I2C_SDA_PIN,GPIO_PIN_RESET)

#define USERI2C_READ_SDA  (HAL_GPIO_ReadPin(I2C_SDA_PORT, I2C_SDA_PIN))

#define I2C_DEVID    			164//0XA4

/* Exported functions ------------------------------------------------------- */
//IIC���в�������
void UserI2c_Init(void);           
void UserI2c_Send_Byte(unsigned char dat);
unsigned char UserI2c_Read_Byte(void);

//unsigned char SensorWritenByte(unsigned char *txbuff, unsigned char regaddr, unsigned char size);
//unsigned char SensorReadnByte(unsigned char *rtxbuff, unsigned char regaddr, unsigned char size);
unsigned char SensorWritenByte(unsigned char Devid, unsigned char *TXBuff, unsigned char SubAdd, unsigned char Size);
unsigned char SensorReadnByte(unsigned char Devid, unsigned char *RXBuff, unsigned char SubAdd, unsigned char Size);

  

   	   		   
//IO��������
#define SDA_IN()  {GPIOB->CRH&=0XFFFFFF0F;GPIOB->CRH|=8<<4;}
#define SDA_OUT() {GPIOB->CRH&=0XFFFFFF0F;GPIOB->CRH|=3<<4;}

//IO��������
#define IIC_SCL_L		GPIOB->BRR |= GPIO_PIN_8
#define IIC_SCL_H		GPIOB->BSRR |= GPIO_PIN_8
#define IIC_SDA_L		GPIOB->BRR |= GPIO_PIN_9
#define IIC_SDA_H		GPIOB->BSRR |= GPIO_PIN_9
#define READ_SDA 		(GPIOB->IDR  & GPIO_PIN_9)

//IIC���в�������
void IIC_Init(void);                //��ʼ��IIC��IO��				 
void IIC_Start(void);				//����IIC��ʼ�ź�
void IIC_Stop(void);	  			//����IICֹͣ�ź�
void IIC_Send_Byte(uint8_t txd);			//IIC����һ���ֽ�
uint8_t IIC_Read_Byte(unsigned char ack);//IIC��ȡһ���ֽ�
uint8_t IIC_Wait_Ack(void); 				//IIC�ȴ�ACK�ź�
void IIC_Ack(void);					//IIC����ACK�ź�
void IIC_NAck(void);				//IIC������ACK�ź�

//void IIC_Write_One_Byte(uint8_t daddr,uint8_t addr,uint8_t data);
//uint8_t IIC_Read_One_Byte(uint8_t daddr,u8 addr);	  
#endif
















