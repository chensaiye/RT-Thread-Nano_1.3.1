#ifndef __OLED_H
#define __OLED_H			  	 
#include "stm32f1xx_hal.h"
#include "stdlib.h"	    
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//Mini STM32������
//SSD1306 OLED ����IC��������
//�޸�����:2019/12/9
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C)
//All rights reserved
////////////////////////////////////////////////////////////////////////////////// 	  

#define OLED_ON_IIC_BUS 		I2C2
#define OLED_SLAVE_ADDRESS	0x78

#define OLED_12864	1		//0.96��
//#define OLED_6432	1				//0.49��

#ifdef OLED_12864
	#define X_WIDTH 	128
	#define Y_WIDTH 	64
#endif

#ifdef OLED_6432
	#define X_WIDTH 	64
	#define Y_WIDTH 	32
#endif
//-----------------OLED IIC????----------------  					   
//OLEDģʽ����
//0:4��iiCģʽ
//1:����8080ģʽ  �ݲ�֧��
#define OLED_MODE 0
////IO????
//#define OLED_SDA_IN()  {GPIOA->CRH&=0XFFFF0FFF;GPIOA->CRH|=4<<12;}
//#define OLED_SDA_OUT() {GPIOA->CRH&=0XFFFF0FFF;GPIOA->CRH|=3<<12;}
////IO????	 
//#define OLED_IIC_SCL    PAout(12) //SCL	12
//#define OLED_IIC_SDA    PAout(11) //SDA	 11
//#define OLED_READ_SDA   PAin(11)  //??SDA 

#define OLED_CMD  0	//д����
#define OLED_DATA 1	//д����

#define CHARSIZE_6X8 8

#define CHARSIZE_8X6 6
#define CHARSIZE_8X8 8
#define CHARSIZE_12X6 12
#define CHARSIZE_16X8 16
#define CHARSIZE_24X12 24
     

//OLED�����ú���
uint16_t OLED_WR_Byte(uint8_t dat,uint8_t cmd);	   

uint16_t OLED_WR_Date(uint16_t WriteAddr,uint8_t *pBuffer,uint16_t NumToWrite);

void OLED_Display_On(void);
void OLED_Display_Off(void);
void OLED_Refresh_Gram(void);		   
							   		    
void OLED_Init(void);
void OLED_Clear(void);
void OLED_DrawPoint(uint8_t x,uint8_t y,uint8_t t);
void OLED_Fill(uint8_t x1,uint8_t y1,uint8_t x2,uint8_t y2,uint8_t dot);
void OLED_ShowChar(uint8_t x,uint8_t y,uint8_t chr,uint8_t size,uint8_t mode);
void OLED_ShowNum(uint8_t x,uint8_t y,uint32_t num,uint8_t len,uint8_t size);
void OLED_ShowString(uint8_t x,uint8_t y,const uint8_t *p,uint8_t size);	

void OLED_Test(void);
int oled_thd_start(void);
	
#endif  
	 



