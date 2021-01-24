#include "cd4051_ctl.h"
#include "panel.h"
/********************************************************************************/
//////////////////////////////////////////////////////////////////////////////////	
//驱动代码
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//STM32开发板
//CSY
//修改日期:
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 启东应成医疗器械有限公司
//All rights reserved									   
//////////////////////////////////////////////////////////////////////////////////
/********************************************************************************/
//修改说明 

////////////////////////////////////////////////////////////////////////////////// 
/********************************************************************************/

//#define __DEBUG
#ifdef __DEBUG
#define DEBUG(format, ...) my_printf(format, ##__VA_ARGS__)
#else
#define DEBUG(format, ...)
#endif

extern u8 SET_FACTORY_FLAG;	//回复出厂设置标志
//extern u16 PLUS_DAC_Table[4][MAX_LEVEL];	//增强型的DAC输出值表		0为DAC1聚焦时数据 1为DAC2聚焦是数据	2为DAC1散开时数据 3为DAC2散开时数据
//extern u16 PLUS_EEP_DAC_Table[8];//PLUS的DAC表最大最小值									0-1									2-3							4-5							6-7	

u16 CD4051_DAC_Table[MAX_SW][4][MAX_LEVEL]={0};

//void Color_Temper_Init(void)
//{
//	
////	if(SET_FACTORY_FLAG == IS_TRUE)
////	{
////		AT24CXX_Write(EEPROM_Vol,(u8 *)CD4051_DAC_Table,sizeof(CD4051_DAC_Table));
////	}
////	
//	AT24CXX_Read(EEPROM_Vol,(u8 *)CD4051_DAC_Table,sizeof(CD4051_DAC_Table));
//	
//}

////主灯亮度设置
//void write_dac2_level(u16 level ,u16 value)
//{
//	if((level==8)||(level==1))
//	{
//		PLUS_DAC_Table[1][level - 1] = value;	//最小值或最大值
//		Dac2_Set_Vol(value);
//	}
//	else
//	{
//		DEBUG("data error\n");
//		return;
//	}
//	
//}
////保存dac2的值
//void save_dac2_level(void)
//{
//	u8 i;
//	u16 step=0;
//	if(PLUS_DAC_Table[1][MIN_LEVEL - 1] >= PLUS_DAC_Table[1][MAX_LEVEL - 1])
//	{
//		DEBUG("data error\n");
//		return;
//	}
//	step = (PLUS_DAC_Table[1][MAX_LEVEL - 1] - PLUS_DAC_Table[1][MIN_LEVEL - 1]) / (MAX_LEVEL -1);
//	for(i = 1; i< (MAX_LEVEL - 1);i++)
//				PLUS_DAC_Table[i][i] = PLUS_DAC_Table[i][i-1] + step;	//保存中间值
//	
//	//保存默认到eeprom
//	PLUS_EEP_DAC_Table[2] = PLUS_DAC_Table[1][MIN_LEVEL - 1];
//	PLUS_EEP_DAC_Table[3] = PLUS_DAC_Table[1][MAX_LEVEL - 1];
//	AT24CXX_WriteOnePage(EEPROM_ARRD2,(u8 *)PLUS_EEP_DAC_Table);
//}
////色温设置
//void write_temp_level(u16 temp,u16 level ,u16 ch0,u16 ch1,u16 ch2,u16 ch3)
//{
//	
//	if((temp>3)||(temp<1)||(level>8)||(level<1))
//	{
//		DEBUG("色温配置错误 %d(色温档位1~3)  %d(亮度等级1~8)\n",temp,level);
//		return;
//	}
//	CD4051_DAC_Table[temp-1][0][level-1]=ch0;
//	CD4051_DAC_Table[temp-1][1][level-1]=ch1;
//	CD4051_DAC_Table[temp-1][2][level-1]=ch2;
//	CD4051_DAC_Table[temp-1][3][level-1]=ch3;
//	SEWEN_FLAG = temp-1;
//	CURRENT_LEVEL = level;
//	DEBUG("set ok");
//}
////色温值保存
//void save_temp_level(void)
//{
//	u16 VOL_Table[3][4][MAX_LEVEL]={0};
//	u16 i,j,k;
//	float chazhi=0;
//	for(i=0;i<3;i++)
//	{
//		for(j=0;j<4;j++)
//		{
//			chazhi=(float)(CD4051_DAC_Table[i][j][7]-CD4051_DAC_Table[i][j][0])/(float)7;
//			for(k=1;k<7;k++)
//			{
//				CD4051_DAC_Table[i][j][k]=CD4051_DAC_Table[i][j][0]+chazhi*(float)k;				
//			}
//		}
//	}
//	
//	
//	AT24CXX_Write(EEPROM_Vol,(u8 *)CD4051_DAC_Table,sizeof(CD4051_DAC_Table));
//	AT24CXX_Read(EEPROM_Vol,(u8 *)VOL_Table,sizeof(VOL_Table));
//	for(i=0;i<3;i++)
//	{
//		for(j=0;j<4;j++)
//		{
//			for(k=0;k<8;k++)
//			{
//				if(VOL_Table[i][j][k]!=CD4051_DAC_Table[i][j][k])
//				{
//					DEBUG("save colorval eeprom failed,%d,%d,%d\n",i,j,k);	
//					return;
//				}else {
//					DEBUG("%d\t",VOL_Table[i][j][k]);
//				}
//			}
//			DEBUG("\r\n");
//		}
//		DEBUG("\r\n");
//	}
//	DEBUG("save eeprom  ok \n");
//}

