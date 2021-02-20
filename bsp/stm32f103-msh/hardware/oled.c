#include "oled.h"
#include "stdlib.h"
#include "oledfont.h"  

#include "iic_bus.h"
#include <rtthread.h>
#include "main_control.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//Mini STM32开发板
//SSD1306 OLED 驱动IC驱动代码
//驱动方式:8080并口/4线串口
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2010/6/3
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 正点原子 2009-2019
//All rights reserved
////////////////////////////////////////////////////////////////////////////////// 	  
 uint8_t oled_error;

//OLED的显存
//存放格式如下.
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


extern pwm_value_union set_val;//设定值
extern union_status curr_status;//当前状态
extern union_para SYS_PARA;	//系统参数
extern uint16_t TOF_Value;
extern uint8_t NRF_ERROR;
//更新显存到LCD		 
void OLED_Refresh_Gram(void)
{
	uint8_t i,n;		    
	for(i=0;i<Y_WIDTH/8;i++)  
	{  
		OLED_WR_Byte (0xb0+i,OLED_CMD);    //设置页地址（0~7）
		OLED_WR_Byte (0x00,OLED_CMD);      //设置显示位置—列低地址
		#ifdef OLED_12864
			OLED_WR_Byte (0x10,OLED_CMD);      //设置显示位置—列高地址  
		#endif
		#ifdef OLED_6432
			OLED_WR_Byte (0x12,OLED_CMD);      //设置显示位置—列高地址  
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
//向SSD1306写入一个字节。
//dat:要写入的数据/命令
//cmd:数据/命令标志 0,表示命令;1,表示数据;
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
//开启OLED显示    
void OLED_Display_On(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC命令
	OLED_WR_Byte(0X14,OLED_CMD);  //DCDC ON
	OLED_WR_Byte(0XAF,OLED_CMD);  //DISPLAY ON
}
//关闭OLED显示     
void OLED_Display_Off(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC命令
	OLED_WR_Byte(0X10,OLED_CMD);  //DCDC OFF
	OLED_WR_Byte(0XAE,OLED_CMD);  //DISPLAY OFF
}		   			 
//清屏函数,清完屏,整个屏幕是黑色的!和没点亮一样!!!	  
void OLED_Clear(void)  
{  
	uint8_t i,n;  
	for(i=0;i<Y_WIDTH/8;i++)for(n=0;n<X_WIDTH;n++)OLED_GRAM[n][i]=0X00;  
	OLED_Refresh_Gram();//更新显示
}
//画点 
//x:0~127
//y:0~63
//t:1 填充 0,清空				   
void OLED_DrawPoint(uint8_t x,uint8_t y,uint8_t t)
{
	uint8_t pos,bx,temp=0;
	if(x>(X_WIDTH-1)||y>(Y_WIDTH-1))return;//超出范围了.
	pos=7-y/8;
	bx=y%8;
	temp=1<<(7-bx);
	if(t)OLED_GRAM[x][pos]|=temp;
	else OLED_GRAM[x][pos]&=~temp;	    
}
//x1,y1,x2,y2 填充区域的对角坐标
//确保x1<=x2;y1<=y2 0<=x1<=127 0<=y1<=63	 	 
//dot:0,清空;1,填充	  
void OLED_Fill(uint8_t x1,uint8_t y1,uint8_t x2,uint8_t y2,uint8_t dot)  
{  
	uint8_t x,y;  
	for(x=x1;x<=x2;x++)
	{
		for(y=y1;y<=y2;y++)OLED_DrawPoint(x,y,dot);
	}													    
	OLED_Refresh_Gram();//更新显示
}
//在指定位置显示一个字符,包括部分字符
//x:0~127
//y:0~63
//mode:0,反白显示;1,正常显示				 
//size:选择字体 16/12 
void OLED_ShowChar(uint8_t x,uint8_t y,uint8_t chr,uint8_t size,uint8_t mode)
{      			    
	uint8_t temp,t,t1;
	uint8_t y0=y;
	uint8_t csize;//=(size/8+((size%8)?1:0))*(size/2);		//得到字体一个字符对应点阵集所占的字节数
	if(size == 6)//CHARSIZE_8X6
		csize = 6;
	else if(size == 8)//CHARSIZE_8X8
		csize = 8;
	else 
		csize=(size/8+((size%8)?1:0))*(size/2);		//得到字体一个字符对应点阵集所占的字节数
	
	chr=chr-' ';//得到偏移后的值		 
    for(t=0;t<csize;t++)
    {   
			if(size==12)temp=asc2_1206[chr][t]; 	 	//调用1206字体
			else if(size==8)temp= asc2_0808[chr][t];	//调用0808字体	
			else if(size==6)temp= asc2_0806[chr][t];	//调用0806字体	
			else if(size==16)temp=asc2_1608[chr][t];	//调用1608字体
			else if(size==24)temp=asc2_2412[chr][t];	//调用2412字体
			else return;								//没有的字库
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
//m^n函数
uint32_t mypow(uint8_t m,uint8_t n)
{
	uint32_t result=1;	 
	while(n--)result*=m;    
	return result;
}				  
//显示2个数字
//x,y :起点坐标	 
//len :数字的位数
//size:字体大小
//mode:模式	0,填充模式;1,叠加模式
//num:数值(0~4294967295);	 		  
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
//显示字符串
//x,y:起点坐标  
//size:字体大小 
//*p:字符串起始地址 
void OLED_ShowString(uint8_t x,uint8_t y,const uint8_t *p,uint8_t size)
{	
		#ifdef OLED_6432
			y=y*size;
		#endif
		while((*p<='~')&&(*p>=' '))//判断是不是非法字符!
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
	MX_I2C2_Init();//IIC初始化
}

void OLED_Read(uint16_t ReadAddr,uint8_t *pBuffer,uint16_t NumToRead)
{
	//I2C_Master_BufferRead(I2C1,pBuffer,NumToRead,0XA0,ReadAddr);
	HAL_I2C_Mem_Read(&hi2c2,OLED_SLAVE_ADDRESS,ReadAddr,I2C_MEMADD_SIZE_8BIT,pBuffer,NumToRead,10000);
}

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

//初始化SSD1306					    
void OLED_Init(void)
{ 	 				 	 					    
	OLED_IIC_Init();
	oled_error = 0;
	rt_thread_mdelay(100);
 	if(OLED_WR_Byte(0xAE,OLED_CMD))		//关闭显示
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
 		  
	OLED_WR_Byte(0xAE,OLED_CMD); //关闭显示
	OLED_WR_Byte(0xD5,OLED_CMD); //设置时钟分频因子,震荡频率
	OLED_WR_Byte(80,OLED_CMD);   //[3:0],分频因子;[7:4],震荡频率
	OLED_WR_Byte(0xA8,OLED_CMD); //设置驱动路数
	OLED_WR_Byte(0X3F,OLED_CMD); //默认0X3F(1/64) 
	OLED_WR_Byte(0xD3,OLED_CMD); //设置显示偏移
	OLED_WR_Byte(0X00,OLED_CMD); //默认为0

	OLED_WR_Byte(0x40,OLED_CMD); //设置显示开始行 [5:0],行数.
													    
	OLED_WR_Byte(0x8D,OLED_CMD); //电荷泵设置
	OLED_WR_Byte(0x14,OLED_CMD); //bit2，开启/关闭
	OLED_WR_Byte(0x20,OLED_CMD); //设置内存地址模式
	OLED_WR_Byte(0x02,OLED_CMD); //[1:0],00，列地址模式;01，行地址模式;10,页地址模式;默认10;
	OLED_WR_Byte(0xA1,OLED_CMD); //段重定义设置,bit0:0,0->0;1,0->127;
	OLED_WR_Byte(0xC0,OLED_CMD); //设置COM扫描方向;bit3:0,普通模式;1,重定义模式 COM[N-1]->COM0;N:驱动路数
	OLED_WR_Byte(0xDA,OLED_CMD); //设置COM硬件引脚配置
	OLED_WR_Byte(0x12,OLED_CMD); //[5:4]配置
		 
	OLED_WR_Byte(0x81,OLED_CMD); //对比度设置
	OLED_WR_Byte(0xEF,OLED_CMD); //1~255;默认0X7F (亮度设置,越大越亮)
	OLED_WR_Byte(0xD9,OLED_CMD); //设置预充电周期
	OLED_WR_Byte(0xf1,OLED_CMD); //[3:0],PHASE 1;[7:4],PHASE 2;
	OLED_WR_Byte(0xDB,OLED_CMD); //设置VCOMH 电压倍率
	OLED_WR_Byte(0x30,OLED_CMD); //[6:4] 000,0.65*vcc;001,0.77*vcc;011,0.83*vcc;

	OLED_WR_Byte(0xA4,OLED_CMD); //全局显示开启;bit0:1,开启;0,关闭;(白屏/黑屏)
	OLED_WR_Byte(0xA6,OLED_CMD); //设置显示方式;bit0:1,反相显示;0,正常显示	    						   
	OLED_WR_Byte(0xAF,OLED_CMD); //开启显示	 
	OLED_Clear();
}  
extern pwm_value_union curr_val;
extern uint16_t ad_value[8];
void OLED_Test(void)
{
	char str[25];
//	OLED_ShowString(32,0,"Run Info",CHARSIZE_8X6);
//	
//	sprintf(str,"Mode:    RIR:    ");
//	OLED_ShowString(0,2*8, (uint8_t *)str,CHARSIZE_8X6);
//	sprintf(str,"Mode:%d   RIR:%d",curr_status.value.mode,curr_status.value.rir);
//	OLED_ShowString(0,1*8, (uint8_t *)str,CHARSIZE_8X6);	
	
	sprintf(str,"Run Info   NRF:   ");
	OLED_ShowString(0,0, (uint8_t *)str,CHARSIZE_8X6);
	sprintf(str,"Run Info   NRF:%d  ",NRF_ERROR);
	OLED_ShowString(0,0, (uint8_t *)str,CHARSIZE_8X6);
	
	sprintf(str,"Mode:  RIR:  Dis:    ");
	OLED_ShowString(0,1*8, (uint8_t *)str,CHARSIZE_8X6);
	sprintf(str,"Mode:%d RIR:%d Dis:%d",curr_status.value.mode,curr_status.value.rir,TOF_Value);
	OLED_ShowString(0,1*8, (uint8_t *)str,CHARSIZE_8X6);	
	
	
	sprintf(str,"Levl:     ");
	OLED_ShowString(0,2*8, (uint8_t *)str,CHARSIZE_8X6);
	sprintf(str,"Levl:%d",curr_status.value.lum_grade);
	OLED_ShowString(0,2*8, (uint8_t *)str,CHARSIZE_8X6);	
	
	sprintf(str,"CSet1:      I1:    ");
	OLED_ShowString(0,3*8, (uint8_t *)str,CHARSIZE_8X6);
	sprintf(str,"CSet1:%d   I1:%d",curr_val.value.ch1,ad_value[0]);
	OLED_ShowString(0,3*8, (uint8_t *)str,CHARSIZE_8X6);
	
	sprintf(str,"CSet2:      I2:    ");
	OLED_ShowString(0,4*8, (uint8_t *)str,CHARSIZE_8X6);
	sprintf(str,"CSet2:%d   I2:%d",curr_val.value.ch2,ad_value[1]);
	OLED_ShowString(0,4*8, (uint8_t *)str,CHARSIZE_8X6);
	
	sprintf(str,"CSet3:      I3:    ");
	OLED_ShowString(0,5*8, (uint8_t *)str,CHARSIZE_8X6);
	sprintf(str,"CSet3:%d   I3:%d",curr_val.value.ch3,ad_value[2]);
	OLED_ShowString(0,5*8, (uint8_t *)str,CHARSIZE_8X6);
	
	sprintf(str,"CSet4:      I7:    ");
	OLED_ShowString(0,6*8, (uint8_t *)str,CHARSIZE_8X6);
	sprintf(str,"CSet4:%d   I7:%d",curr_val.value.ch4,ad_value[6]);
	OLED_ShowString(0,6*8, (uint8_t *)str,CHARSIZE_8X6);
	
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


static void oled_display_entey(void *parameter)
{
	//OLED_Init();
	while(1)
  {
		rt_thread_mdelay(1000);
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

























