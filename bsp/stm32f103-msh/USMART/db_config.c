#include "db.h"
#include "db_str.h"
////////////////////////////用户配置区///////////////////////////////////////////////
//这下面要包含所用到的函数所申明的头文件(用户自己添加) 
//#include "delay.h"	
//#include "usart.h"		
//#include "sys.h"

#include "main_control.h"

//函数名列表初始化(用户自己添加)
//用户直接在这里输入要执行的函数名及其查找串
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
//函数控制管理器初始化
//得到各个受控函数的名字
//得到函数总数量
struct _m_db_dev db_dev=
{
	db_nametab,
	db_init,
	db_cmd_rec,
	db_exe,
	db_scan,
	sizeof(db_nametab)/sizeof(struct _m_db_nametab),//函数数量
	0,	  	//参数数量
	0,	 	//函数ID
	1,		//参数显示类型,0,10进制;1,16进制
	0,		//参数类型.bitx:,0,数字;1,字符串	    
	0,	  	//每个参数的长度暂存表,需要MAX_PARM个0初始化
	0,		//函数的参数,需要PARM_LEN个0初始化
};   



