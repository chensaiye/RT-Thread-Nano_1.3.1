#include "db.h"
#include "db_str.h"
////////////////////////////�û�������///////////////////////////////////////////////
//������Ҫ�������õ��ĺ�����������ͷ�ļ�(�û��Լ����) 
//#include "delay.h"	
//#include "usart.h"		
//#include "sys.h"

#include "main_control.h"

//�������б��ʼ��(�û��Լ����)
//�û�ֱ������������Ҫִ�еĺ�����������Ҵ�
struct _m_db_nametab db_nametab[]=
{
	(void*)Event_Power,"void Event_Power(void)",
	(void*)Event_Mode,"void Event_Mode(void)",	
	(void*)Event_Add,"void Event_Add(void)",
	(void*)Event_Minus,"void Event_Minus(void)",
		
	(void*)Set_V4,"Set_V4(uint16_t ch1,uint16_t ch2,uint16_t ch3,uint16_t ch4)",
	(void*)Set_Red,"Set_Red(uint16_t ch5)",
	(void*)Save_To_MIN,"void Save_To_MIN(uint16_t mode_no)",
	(void*)Save_To_MAX,"void Save_To_MAX(uint16_t mode_no)",
	(void*)Set_Gain,"void Set_Gain(uint16_t data)",
	(void*)Save_Gain,"void Save_Gain(void)",
	
	
};						  

///////////////////////////////////END///////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
//�������ƹ�������ʼ��
//�õ������ܿغ���������
//�õ�����������
struct _m_db_dev db_dev=
{
	db_nametab,
	db_init,
	db_cmd_rec,
	db_exe,
	db_scan,
	sizeof(db_nametab)/sizeof(struct _m_db_nametab),//��������
	0,	  	//��������
	0,	 	//����ID
	1,		//������ʾ����,0,10����;1,16����
	0,		//��������.bitx:,0,����;1,�ַ���	    
	0,	  	//ÿ�������ĳ����ݴ��,��ҪMAX_PARM��0��ʼ��
	0,		//�����Ĳ���,��ҪPARM_LEN��0��ʼ��
};   



