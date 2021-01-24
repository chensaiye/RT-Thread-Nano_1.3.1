#include "cd4051_ctl.h"
#include "panel.h"
/********************************************************************************/
//////////////////////////////////////////////////////////////////////////////////	
//��������
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//STM32������
//CSY
//�޸�����:
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ����Ӧ��ҽ����е���޹�˾
//All rights reserved									   
//////////////////////////////////////////////////////////////////////////////////
/********************************************************************************/
//�޸�˵�� 

////////////////////////////////////////////////////////////////////////////////// 
/********************************************************************************/

//#define __DEBUG
#ifdef __DEBUG
#define DEBUG(format, ...) my_printf(format, ##__VA_ARGS__)
#else
#define DEBUG(format, ...)
#endif

extern u8 SET_FACTORY_FLAG;	//�ظ��������ñ�־
//extern u16 PLUS_DAC_Table[4][MAX_LEVEL];	//��ǿ�͵�DAC���ֵ��		0ΪDAC1�۽�ʱ���� 1ΪDAC2�۽�������	2ΪDAC1ɢ��ʱ���� 3ΪDAC2ɢ��ʱ����
//extern u16 PLUS_EEP_DAC_Table[8];//PLUS��DAC�������Сֵ									0-1									2-3							4-5							6-7	

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

////������������
//void write_dac2_level(u16 level ,u16 value)
//{
//	if((level==8)||(level==1))
//	{
//		PLUS_DAC_Table[1][level - 1] = value;	//��Сֵ�����ֵ
//		Dac2_Set_Vol(value);
//	}
//	else
//	{
//		DEBUG("data error\n");
//		return;
//	}
//	
//}
////����dac2��ֵ
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
//				PLUS_DAC_Table[i][i] = PLUS_DAC_Table[i][i-1] + step;	//�����м�ֵ
//	
//	//����Ĭ�ϵ�eeprom
//	PLUS_EEP_DAC_Table[2] = PLUS_DAC_Table[1][MIN_LEVEL - 1];
//	PLUS_EEP_DAC_Table[3] = PLUS_DAC_Table[1][MAX_LEVEL - 1];
//	AT24CXX_WriteOnePage(EEPROM_ARRD2,(u8 *)PLUS_EEP_DAC_Table);
//}
////ɫ������
//void write_temp_level(u16 temp,u16 level ,u16 ch0,u16 ch1,u16 ch2,u16 ch3)
//{
//	
//	if((temp>3)||(temp<1)||(level>8)||(level<1))
//	{
//		DEBUG("ɫ�����ô��� %d(ɫ�µ�λ1~3)  %d(���ȵȼ�1~8)\n",temp,level);
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
////ɫ��ֵ����
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

