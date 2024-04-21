#include "oled.h"
#include "stdlib.h"
#include "oledfont.h"  

#include "iic_bus.h"
#include <rtthread.h>
#include "main_control.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ���������ɣ��������������κ���;
//Mini STM32������
//SSD1306 OLED ����IC��������
//������ʽ:8080����/4�ߴ���
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2010/6/3
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ����ԭ�� 2009-2019
//All rights reserved
////////////////////////////////////////////////////////////////////////////////// 	  
 uint8_t oled_error;

//OLED���Դ�
//��Ÿ�ʽ����.
//[0]0 1 2 3 ... 127	
//[1]0 1 2 3 ... 127	
//[2]0 1 2 3 ... 127	
//[3]0 1 2 3 ... 127	
//[4]0 1 2 3 ... 127	
//[5]0 1 2 3 ... 127	
//[6]0 1 2 3 ... 127	
//[7]0 1 2 3 ... 127 		   
uint8_t OLED_GRAM[X_WIDTH][Y_WIDTH/8];	 

extern I2C_HandleTypeDef hi2c2;


extern pwm_value_union set_val;//�趨ֵ
extern union_status curr_status;//��ǰ״̬
extern union_para SYS_PARA;	//ϵͳ����
extern uint16_t TOF_Value;
extern uint8_t NRF_ERROR;
//�����Դ浽LCD		 
void OLED_Refresh_Gram(void)
{
	uint8_t i,n;		    
	for(i=0;i<Y_WIDTH/8;i++)  
	{  
		OLED_WR_Byte (0xb0+i,OLED_CMD);    //����ҳ��ַ��0~7��
		OLED_WR_Byte (0x00,OLED_CMD);      //������ʾλ�á��е͵�ַ
		#ifdef OLED_12864
			OLED_WR_Byte (0x10,OLED_CMD);      //������ʾλ�á��иߵ�ַ  
		#endif
		#ifdef OLED_6432
			OLED_WR_Byte (0x12,OLED_CMD);      //������ʾλ�á��иߵ�ַ  
		#endif
		for(n=0;n<X_WIDTH;n++)OLED_WR_Byte(OLED_GRAM[n][i],OLED_DATA); 
		//OLED_WR_Date(0,OLED_GRAM[i],X_WIDTH);
	}   
}

/**********************************************
// IIC Write byte
**********************************************/

uint16_t Write_IIC_Byte(unsigned char IIC_Byte)
{
	uint16_t error_no=0;
	//HAL_I2C_Master_Transmit(&hi2c2,OLED_SLAVE_ADDRESS,&IIC_Byte,1,10000);
	error_no =HAL_I2C_Mem_Write(&hi2c2,OLED_SLAVE_ADDRESS,0,I2C_MEMADD_SIZE_8BIT,&IIC_Byte,1,10000);
	return error_no;
}
/**********************************************
// IIC Write Command
**********************************************/
uint16_t Write_IIC_Command(unsigned char IIC_Command)
{		
	uint8_t buf[2];
	uint16_t error_no=0;
	buf[0] = 0x00;
	buf[1] = IIC_Command;
		//HAL_I2C_Mem_Write(&hi2c2,OLED_SLAVE_ADDRESS,0,I2C_MEMADD_SIZE_8BIT,buf,2,10000);
	error_no = HAL_I2C_Master_Transmit(&hi2c2,OLED_SLAVE_ADDRESS,buf,2,10000);
	return error_no;
}
/**********************************************
// IIC Write Data
**********************************************/
uint16_t Write_IIC_Data(unsigned char IIC_Data)
{
	uint8_t buf[2];
	uint16_t error_no=0;
	buf[0] = 0x40;
	buf[1] = IIC_Data;
//HAL_I2C_Mem_Write(&hi2c2,OLED_SLAVE_ADDRESS,0,I2C_MEMADD_SIZE_8BIT,buf,2,10000);
	error_no = HAL_I2C_Master_Transmit(&hi2c2,OLED_SLAVE_ADDRESS,buf,2,10000);
	return error_no;
}

/**********************************************
//OLED Write Byte
**********************************************/
//��SSD1306д��һ���ֽڡ�
//dat:Ҫд�������/����
//cmd:����/�����־ 0,��ʾ����;1,��ʾ����;
uint16_t OLED_WR_Byte(uint8_t dat,uint8_t cmd)
{	
	uint16_t error_no=0;
	if(cmd){
		error_no = Write_IIC_Data(dat);
  }
	else {
		error_no = Write_IIC_Command(dat);
	}
	return error_no;
} 
uint16_t OLED_WR_Date(uint16_t WriteAddr,uint8_t *pBuffer,uint16_t NumToWrite)
{
	uint16_t error_no=0;
	error_no =HAL_I2C_Mem_Write(&hi2c2,OLED_SLAVE_ADDRESS,WriteAddr,I2C_MEMADD_SIZE_8BIT,pBuffer,NumToWrite,10000);
	return error_no;
}	
//����OLED��ʾ    
void OLED_Display_On(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC����
	OLED_WR_Byte(0X14,OLED_CMD);  //DCDC ON
	OLED_WR_Byte(0XAF,OLED_CMD);  //DISPLAY ON
}
//�ر�OLED��ʾ     
void OLED_Display_Off(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC����
	OLED_WR_Byte(0X10,OLED_CMD);  //DCDC OFF
	OLED_WR_Byte(0XAE,OLED_CMD);  //DISPLAY OFF
}		   			 
//��������,������,������Ļ�Ǻ�ɫ��!��û����һ��!!!	  
void OLED_Clear(void)  
{  
	uint8_t i,n;  
	for(i=0;i<Y_WIDTH/8;i++)for(n=0;n<X_WIDTH;n++)OLED_GRAM[n][i]=0X00;  
	OLED_Refresh_Gram();//������ʾ
}
//���� 
//x:0~127
//y:0~63
//t:1 ��� 0,���				   
void OLED_DrawPoint(uint8_t x,uint8_t y,uint8_t t)
{
	uint8_t pos,bx,temp=0;
	if(x>(X_WIDTH-1)||y>(Y_WIDTH-1))return;//������Χ��.
	pos=7-y/8;
	bx=y%8;
	temp=1<<(7-bx);
	if(t)OLED_GRAM[x][pos]|=temp;
	else OLED_GRAM[x][pos]&=~temp;	    
}
//x1,y1,x2,y2 �������ĶԽ�����
//ȷ��x1<=x2;y1<=y2 0<=x1<=127 0<=y1<=63	 	 
//dot:0,���;1,���	  
void OLED_Fill(uint8_t x1,uint8_t y1,uint8_t x2,uint8_t y2,uint8_t dot)  
{  
	uint8_t x,y;  
	for(x=x1;x<=x2;x++)
	{
		for(y=y1;y<=y2;y++)OLED_DrawPoint(x,y,dot);
	}													    
	OLED_Refresh_Gram();//������ʾ
}
//��ָ��λ����ʾһ���ַ�,���������ַ�
//x:0~127
//y:0~63
//mode:0,������ʾ;1,������ʾ				 
//size:ѡ������ 16/12 
void OLED_ShowChar(uint8_t x,uint8_t y,uint8_t chr,uint8_t size,uint8_t mode)
{      			    
	uint8_t temp,t,t1;
	uint8_t y0=y;
	uint8_t csize;//=(size/8+((size%8)?1:0))*(size/2);		//�õ�����һ���ַ���Ӧ������ռ���ֽ���
	if(size == 6)//CHARSIZE_8X6
		csize = 6;
	else if(size == 8)//CHARSIZE_8X8
		csize = 8;
	else 
		csize=(size/8+((size%8)?1:0))*(size/2);		//�õ�����һ���ַ���Ӧ������ռ���ֽ���
	
	chr=chr-' ';//�õ�ƫ�ƺ��ֵ		 
    for(t=0;t<csize;t++)
    {   
			if(size==12)temp=asc2_1206[chr][t]; 	 	//����1206����
			else if(size==8)temp= asc2_0808[chr][t];	//����0808����	
			else if(size==6)temp= asc2_0806[chr][t];	//����0806����	
			else if(size==16)temp=asc2_1608[chr][t];	//����1608����
			else if(size==24)temp=asc2_2412[chr][t];	//����2412����
			else return;								//û�е��ֿ�
			for(t1=0;t1<8;t1++)
			{
				if(temp&0x80)OLED_DrawPoint(x,y,mode);
				else OLED_DrawPoint(x,y,!mode);
				temp<<=1;
				y++;
				if(size == CHARSIZE_8X6)
				{
					if((y-y0)== 8)
					{
						y=y0;
						x++;
						break;
					}
				}
				else
				{
					if((y-y0)==size)
					{
						y=y0;
						x++;
						break;
					}
				}
			}  	 
    }          
}
//m^n����
uint32_t mypow(uint8_t m,uint8_t n)
{
	uint32_t result=1;	 
	while(n--)result*=m;    
	return result;
}				  
//��ʾ2������
//x,y :�������	 
//len :���ֵ�λ��
//size:�����С
//mode:ģʽ	0,���ģʽ;1,����ģʽ
//num:��ֵ(0~4294967295);	 		  
void OLED_ShowNum(uint8_t x,uint8_t y,uint32_t num,uint8_t len,uint8_t size)
{         	
	uint8_t t,temp;
	uint8_t enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/mypow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				OLED_ShowChar(x+(size/2)*t,y,' ',size,1);
				continue;
			}else enshow=1; 
		 	 
		}
	 	OLED_ShowChar(x+(size/2)*t,y,temp+'0',size,1); 
	}
} 
//��ʾ�ַ���
//x,y:�������  
//size:�����С 
//*p:�ַ�����ʼ��ַ 
void OLED_ShowString(uint8_t x,uint8_t y,const uint8_t *p,uint8_t size)
{	
		#ifdef OLED_6432
			y=y*size;
		#endif
		while((*p<='~')&&(*p>=' '))//�ж��ǲ��ǷǷ��ַ�!
    {       
        if(size == CHARSIZE_8X8)
				{
					if(x>(X_WIDTH-size)){x=0;y+=size;}
					if(y>(Y_WIDTH-size)){y=x=0;OLED_Clear();}
				}
				else if(size == CHARSIZE_8X6)
				{
					if(x>(X_WIDTH-size)){x=0;y+=size;}
					if(y>(Y_WIDTH-8)){y=x=0;OLED_Clear();}
				}
				else
				{
					if(x>(X_WIDTH-(size/2))){x=0;y+=size;}
					if(y>(Y_WIDTH-size)){y=x=0;OLED_Clear();}
				}
				
        OLED_ShowChar(x,y,*p,size,1);	 
				if(size < 12)
					x+=size;
				else
					x+=size/2;
        p++;
    }  
	
}	

void OLED_IIC_Init(void)
{	
	MX_I2C2_Init();//IIC��ʼ��
}

//void OLED_Read(uint16_t ReadAddr,uint8_t *pBuffer,uint16_t NumToRead)
//{
//	//I2C_Master_BufferRead(I2C1,pBuffer,NumToRead,0XA0,ReadAddr);
//	HAL_I2C_Mem_Read(&hi2c2,OLED_SLAVE_ADDRESS,ReadAddr,I2C_MEMADD_SIZE_8BIT,pBuffer,NumToRead,10000);
//}

/**********************************************
// IIC Write Data
**********************************************/
uint8_t Write_IIC_Command_Test(unsigned char IIC_Command)
{

//   OLED_IIC_Start();
//   Write_IIC_Byte(0x78);			//D/C#=0; R/W#=0
//	 if(OLED_IIC_Wait_Ack())
//		 return 1;//error	
//   Write_IIC_Byte(0x00);			//write Command
//	  if(OLED_IIC_Wait_Ack())
//		 return 1;//error		
//   Write_IIC_Byte(IIC_Command);
//	  if(OLED_IIC_Wait_Ack())
//		 return 1;//error		
//   OLED_IIC_Stop();
	return 0;//error	
}

//��ʼ��SSD1306					    
void OLED_Init(void)
{ 	 				 	 					    
	OLED_IIC_Init();
	oled_error = 0;
	rt_thread_mdelay(100);
 	if(OLED_WR_Byte(0xAE,OLED_CMD))		//�ر���ʾ
	{
		oled_error = 1;
		return;
	}
//	if(Write_IIC_Command_Test(0xAE))
//	{
//		oled_error = 1;
//		return;
//	}
//	else
//		oled_error = 0;								  
 		  
	OLED_WR_Byte(0xAE,OLED_CMD); //�ر���ʾ
	OLED_WR_Byte(0xD5,OLED_CMD); //����ʱ�ӷ�Ƶ����,��Ƶ��
	OLED_WR_Byte(80,OLED_CMD);   //[3:0],��Ƶ����;[7:4],��Ƶ��
	OLED_WR_Byte(0xA8,OLED_CMD); //��������·��
	OLED_WR_Byte(0X3F,OLED_CMD); //Ĭ��0X3F(1/64) 
	OLED_WR_Byte(0xD3,OLED_CMD); //������ʾƫ��
	OLED_WR_Byte(0X00,OLED_CMD); //Ĭ��Ϊ0

	OLED_WR_Byte(0x40,OLED_CMD); //������ʾ��ʼ�� [5:0],����.
													    
	OLED_WR_Byte(0x8D,OLED_CMD); //��ɱ�����
	OLED_WR_Byte(0x14,OLED_CMD); //bit2������/�ر�
	OLED_WR_Byte(0x20,OLED_CMD); //�����ڴ��ַģʽ
	OLED_WR_Byte(0x02,OLED_CMD); //[1:0],00���е�ַģʽ;01���е�ַģʽ;10,ҳ��ַģʽ;Ĭ��10;
	OLED_WR_Byte(0xA1,OLED_CMD); //���ض�������,bit0:0,0->0;1,0->127;
	OLED_WR_Byte(0xC0,OLED_CMD); //����COMɨ�跽��;bit3:0,��ͨģʽ;1,�ض���ģʽ COM[N-1]->COM0;N:����·��
	OLED_WR_Byte(0xDA,OLED_CMD); //����COMӲ����������
	OLED_WR_Byte(0x12,OLED_CMD); //[5:4]����
		 
	OLED_WR_Byte(0x81,OLED_CMD); //�Աȶ�����
	OLED_WR_Byte(0xEF,OLED_CMD); //1~255;Ĭ��0X7F (��������,Խ��Խ��)
	OLED_WR_Byte(0xD9,OLED_CMD); //����Ԥ�������
	OLED_WR_Byte(0xf1,OLED_CMD); //[3:0],PHASE 1;[7:4],PHASE 2;
	OLED_WR_Byte(0xDB,OLED_CMD); //����VCOMH ��ѹ����
	OLED_WR_Byte(0x30,OLED_CMD); //[6:4] 000,0.65*vcc;001,0.77*vcc;011,0.83*vcc;

	OLED_WR_Byte(0xA4,OLED_CMD); //ȫ����ʾ����;bit0:1,����;0,�ر�;(����/����)
	OLED_WR_Byte(0xA6,OLED_CMD); //������ʾ��ʽ;bit0:1,������ʾ;0,������ʾ	    						   
	OLED_WR_Byte(0xAF,OLED_CMD); //������ʾ	 
	OLED_Clear();
}  
extern pwm_value_union curr_val;
extern uint16_t ad_value[8];
uint16_t temperature_display;
void OLED_Test(void)
{
	char str[25];
//	OLED_ShowString(32,0,"Run Info",CHARSIZE_8X6);
//	
//	sprintf(str,"Mode:    RIR:    ");
//	OLED_ShowString(0,2*8, (uint8_t *)str,CHARSIZE_8X6);
//	sprintf(str,"Mode:%d   RIR:%d",curr_status.value.mode,curr_status.value.rir);
//	OLED_ShowString(0,1*8, (uint8_t *)str,CHARSIZE_8X6);	
	
	//sprintf(str,"6Touch 10Lvl NRF:   ");
	//OLED_ShowString(0,0, (uint8_t *)str,CHARSIZE_8X6);
	//sprintf(str,"6Touch 10Lvl NRF:%d  ",NRF_ERROR);
	sprintf(str,"4.10_10L_NO_RED NRF:%d",NRF_ERROR); //length limited 21
	//sprintf(str,"4.10_10L_RED NRF:%d",NRF_ERROR);
	OLED_ShowString(0,0, (uint8_t *)str,CHARSIZE_8X6);
	
	sprintf(str,"Mode:  RIR:  Dis:    ");
	OLED_ShowString(0,1*8, (uint8_t *)str,CHARSIZE_8X6);
	sprintf(str,"Mode:%d RIR:%d Dis:%d",curr_status.value.mode,curr_status.value.rir,TOF_Value);
	OLED_ShowString(0,1*8, (uint8_t *)str,CHARSIZE_8X6);	
	
	
	sprintf(str,"Levl:     ");
	OLED_ShowString(0,2*8, (uint8_t *)str,CHARSIZE_8X6);
	sprintf(str,"Levl:%d",curr_status.value.lum_grade);
	OLED_ShowString(0,2*8, (uint8_t *)str,CHARSIZE_8X6);
		
	sprintf(str,"TEMP:     ");
	OLED_ShowString(54,2*8, (uint8_t *)str,CHARSIZE_8X6);
	sprintf(str,"TEMP:%d",temperature_display);
	OLED_ShowString(54,2*8, (uint8_t *)str,CHARSIZE_8X6);	
	
	sprintf(str,"CS1:     ");
	OLED_ShowString(0,3*8, (uint8_t *)str,CHARSIZE_8X6);
	sprintf(str,"CS1:%d ",curr_val.value.ch1);
	OLED_ShowString(0,3*8, (uint8_t *)str,CHARSIZE_8X6);
	sprintf(str,"CS2:     ");
	OLED_ShowString(0,4*8, (uint8_t *)str,CHARSIZE_8X6);
	sprintf(str,"CS2:%d ",curr_val.value.ch2);
	OLED_ShowString(0,4*8, (uint8_t *)str,CHARSIZE_8X6);
	sprintf(str,"CS3:     ");
	OLED_ShowString(0,5*8, (uint8_t *)str,CHARSIZE_8X6);
	sprintf(str,"CS3:%d ",curr_val.value.ch3);
	OLED_ShowString(0,5*8, (uint8_t *)str,CHARSIZE_8X6);
	sprintf(str,"CS4:     ");
	OLED_ShowString(0,6*8, (uint8_t *)str,CHARSIZE_8X6);
	sprintf(str,"CS4:%d ",curr_val.value.ch4);
	OLED_ShowString(0,6*8, (uint8_t *)str,CHARSIZE_8X6);
	
	sprintf(str,"CS5:     ");
	OLED_ShowString(0,7*8, (uint8_t *)str,CHARSIZE_8X6);
	sprintf(str,"CS5:%d ",curr_val.value.ch5);
	OLED_ShowString(0,7*8, (uint8_t *)str,CHARSIZE_8X6);
	
	sprintf(str,"I1:     ");
	OLED_ShowString(54,3*8, (uint8_t *)str,CHARSIZE_8X6);
	sprintf(str,"I1:%d ",ad_value[0]);
	OLED_ShowString(54,3*8, (uint8_t *)str,CHARSIZE_8X6);
	sprintf(str,"I2:     ");
	OLED_ShowString(54,4*8, (uint8_t *)str,CHARSIZE_8X6);
	sprintf(str,"I2:%d ",ad_value[1]);
	OLED_ShowString(54,4*8, (uint8_t *)str,CHARSIZE_8X6);
	sprintf(str,"I3:     ");
	OLED_ShowString(54,5*8, (uint8_t *)str,CHARSIZE_8X6);
	sprintf(str,"I3:%d ",ad_value[2]);
	OLED_ShowString(54,5*8, (uint8_t *)str,CHARSIZE_8X6);
	sprintf(str,"I7:     ");
	OLED_ShowString(54,6*8, (uint8_t *)str,CHARSIZE_8X6);
	sprintf(str,"I7:%d ",ad_value[6]);
	OLED_ShowString(54,6*8, (uint8_t *)str,CHARSIZE_8X6);
	
//	sprintf(str,"I4:    ");
//	OLED_ShowString(0,7*8, (uint8_t *)str,CHARSIZE_8X6);
//	sprintf(str,"I4:%d",ad_value[3]);
//	OLED_ShowString(0,7*8, (uint8_t *)str,CHARSIZE_8X6);
//	sprintf(str,"I5:    ");
//	OLED_ShowString(42,7*8, (uint8_t *)str,CHARSIZE_8X6);
//	sprintf(str,"I5:%d",ad_value[4]);
//	OLED_ShowString(42,7*8, (uint8_t *)str,CHARSIZE_8X6);
//	sprintf(str,"I6:    ");
//	OLED_ShowString(84,7*8, (uint8_t *)str,CHARSIZE_8X6);
//	sprintf(str,"I6:%d",ad_value[5]);
//	OLED_ShowString(84,7*8, (uint8_t *)str,CHARSIZE_8X6);
	
//	sprintf(str,"CSet1:      I1:    ");
//	OLED_ShowString(0,3*8, (uint8_t *)str,CHARSIZE_8X6);
//	sprintf(str,"CSet1:%d   I1:%d",curr_val.value.ch1,ad_value[0]);
//	OLED_ShowString(0,3*8, (uint8_t *)str,CHARSIZE_8X6);
//	
//	sprintf(str,"CSet2:      I2:    ");
//	OLED_ShowString(0,4*8, (uint8_t *)str,CHARSIZE_8X6);
//	sprintf(str,"CSet2:%d   I2:%d",curr_val.value.ch2,ad_value[1]);
//	OLED_ShowString(0,4*8, (uint8_t *)str,CHARSIZE_8X6);
//	
//	sprintf(str,"CSet3:      I3:    ");
//	OLED_ShowString(0,5*8, (uint8_t *)str,CHARSIZE_8X6);
//	sprintf(str,"CSet3:%d   I3:%d",curr_val.value.ch3,ad_value[2]);
//	OLED_ShowString(0,5*8, (uint8_t *)str,CHARSIZE_8X6);
//	
//	sprintf(str,"CSet4:      I7:    ");
//	OLED_ShowString(0,6*8, (uint8_t *)str,CHARSIZE_8X6);
//	sprintf(str,"CSet4:%d   I7:%d",curr_val.value.ch4,ad_value[6]);
//	OLED_ShowString(0,6*8, (uint8_t *)str,CHARSIZE_8X6);
	
//	sprintf(str,"Distence:   ");
//	OLED_ShowString(0,7*8, (uint8_t *)str,CHARSIZE_8X6);
//	sprintf(str,"Distence:%d",TOF_Value);
//	OLED_ShowString(0,7*8, (uint8_t *)str,CHARSIZE_8X6);
		
	OLED_Refresh_Gram();
}


#define OLED_THREAD_PRIORITY         (RT_THREAD_PRIORITY_MAX - 3)
#define OLED_THREAD_STACK_SIZE       512
#define OLED_THREAD_TIMESLICE        20

ALIGN(RT_ALIGN_SIZE)
static char thd_oled_stack[OLED_THREAD_STACK_SIZE];
static struct rt_thread thd_oled;

#include "LM75A.h"
static void oled_display_entey(void *parameter)
{
	//OLED_Init();
	while(1)
  {
		rt_thread_mdelay(1000);
		temperature_display = LM75A_Read_Test();
		if(oled_error == 0)
			OLED_Test();
		
		
	}
}

int oled_thd_start(void)
{
		OLED_Init();
		if(oled_error )
			return 1;
		 rt_thread_init(&thd_oled,
										 "oled_dis",
										 oled_display_entey,
										 RT_NULL,
										 &thd_oled_stack[0],
										 sizeof(thd_oled_stack),
										 OLED_THREAD_PRIORITY-1, OLED_THREAD_TIMESLICE);
										 
		 rt_thread_startup(&thd_oled);
	
    return 0;
}

MSH_CMD_EXPORT(oled_thd_start, thread oled);

























