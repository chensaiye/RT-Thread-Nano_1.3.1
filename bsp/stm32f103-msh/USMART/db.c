#include "db.h"
#include <rtthread.h>

//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com 
//版本：V3.1
//版权所有，盗版必究。
//Copyright(C) 正点原子 2011-2021
//All rights reserved
//********************************************************************************
//升级说明
//V1.4
//增加了对参数为string类型的函数的支持.适用范围大大提高.
//优化了内存占用,静态内存占用为79个字节@10个参数.动态适应数字及字符串长度
//V2.0 
//1,修改了list指令,打印函数的完整表达式.
//2,增加了id指令,打印每个函数的入口地址.
//3,修改了参数匹配,支持函数参数的调用(输入入口地址).
//4,增加了函数名长度宏定义.	
//V2.1 20110707		 
//1,增加dec,hex两个指令,用于设置参数显示进制,及执行进制转换.
//注:当dec,hex不带参数的时候,即设定显示参数进制.当后跟参数的时候,即执行进制转换.
//如:"dec 0XFF" 则会将0XFF转为255,由串口返回.
//如:"hex 100" 	则会将100转为0X64,由串口返回
//2,新增db_get_cmdname函数,用于获取指令名字.
//V2.2 20110726	
//1,修正了void类型参数的参数统计错误.
//2,修改数据显示格式默认为16进制.
//V2.3 20110815
//1,去掉了函数名后必须跟"("的限制.
//2,修正了字符串参数中不能有"("的bug.
//3,修改了函数默认显示参数格式的修改方式. 
//V2.4 20110905
//1,修改了db_get_cmdname函数,增加最大参数长度限制.避免了输入错误参数时的死机现象.
//2,增加DB_ENTIM_SCAN宏定义,用于配置是否使用TIM定时执行scan函数.
//V2.5 20110930
//1,修改db_init函数为void db_init(u8 sysclk),可以根据系统频率自动设定扫描时间.(固定100ms)
//2,去掉了db_init函数中的uart_init函数,串口初始化必须在外部初始化,方便用户自行管理.
//V2.6 20111009
//1,增加了read_addr和write_addr两个函数.可以利用这两个函数读写内部任意地址(必须是有效地址).更加方便调试.
//2,read_addr和write_addr两个函数可以通过设置DB_USE_WRFUNS为来使能和关闭.
//3,修改了db_strcmp,使其规范化.			  
//V2.7 20111024
//1,修正了返回值16进制显示时不换行的bug.
//2,增加了函数是否有返回值的判断,如果没有返回值,则不会显示.有返回值时才显示其返回值.
//V2.8 20111116
//1,修正了list等不带参数的指令发送后可能导致死机的bug.
//V2.9 20120917
//1,修改了形如：void*xxx(void)类型函数不能识别的bug。
//V3.0 20130425
//1,新增了字符串参数对转义符的支持。
//V3.1 20131120
//1,增加runtime系统指令,可以用于统计函数执行时间.
//用法:
//发送:runtime 1 ,则开启函数执行时间统计功能
//发送:runtime 0 ,则关闭函数执行时间统计功能
///runtime统计功能,必须设置:DB_ENTIMX_SCAN 为1,才可以使用!!
/////////////////////////////////////////////////////////////////////////////////////
//DB资源占用情况@MDK 3.80A@2.0版本：
//FLASH:4K~K字节(通过DB_USE_HELP和DB_USE_WRFUNS设置)
//SRAM:72字节(最少的情况下)
//SRAM计算公式:   SRAM=PARM_LEN+72-4  其中PARM_LEN必须大于等于4.
//应该保证堆栈不小于100个字节.
////////////////////////////////////////////用户配置参数////////////////////////////////////////////////////	  

#define __DEBUG
#ifdef __DEBUG
#define DEBUG(format, ...) rt_kprintf(format, ##__VA_ARGS__)
#else
#define DEBUG(format, ...)
#endif	    

////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////
//初始化串口控制器
//sysclk:系统时钟（Mhz）
void db_init(u8 sysclk)
{

	db_dev.sptype=1;	//十六进制显示参数
}		
//从str中获取函数名,id,及参数信息
//*str:字符串指针.
//返回值:0,识别成功;其他,错误代码.
u8 db_cmd_rec(u8*str) 
{
	u8 sta,i,rval;//状态	 
	u8 rpnum,spnum;
	u8 rfname[MAX_DB_FNAME_LEN];//暂存空间,用于存放接收到的函数名  
	u8 sfname[MAX_DB_FNAME_LEN];//存放本地函数名
	sta=db_get_fname(str,rfname,&rpnum,&rval);//得到接收到的数据的函数名及参数个数	  
	if(sta)return sta;//错误
	for(i=0;i<db_dev.fnum;i++)
	{
		sta=db_get_fname((u8*)db_dev.funs[i].name,sfname,&spnum,&rval);//得到本地函数名及参数个数
		if(sta)return sta;//本地解析有误	  
		if(db_strcmp(sfname,rfname)==0)//相等
		{
			if(spnum>rpnum)return DB_PARMERR;//参数错误(输入参数比源函数参数少)
			db_dev.id=i;//记录函数ID.
			break;//跳出.
		}	
	}
	if(i==db_dev.fnum)return DB_NOFUNCFIND;	//未找到匹配的函数
 	sta=db_get_fparam(str,&i);					//得到函数参数个数	
	if(sta)return sta;								//返回错误
	db_dev.pnum=i;								//参数个数记录
    return DB_OK;
}
//usamrt执行函数
//该函数用于最终执行从串口收到的有效函数.
//最多支持10个参数的函数,更多的参数支持也很容易实现.不过用的很少.一般5个左右的参数的函数已经很少见了.
//该函数会在串口打印执行情况.以:"函数名(参数1，参数2...参数N)=返回值".的形式打印.
//当所执行的函数没有返回值的时候,所打印的返回值是一个无意义的数据.
void db_exe(void)
{
	u8 id,i;
	u32 res=0;		   
	u32 temp[MAX_DB_PARM_NO];//参数转换,使之支持了字符串 
	u8 sfname[MAX_DB_FNAME_LEN];//存放本地函数名
	u8 pnum,rval;
	id=db_dev.id;
	if(id>=db_dev.fnum)return;//不执行.
	db_get_fname((u8*)db_dev.funs[id].name,sfname,&pnum,&rval);//得到本地函数名,及参数个数 
	DEBUG("\r\n%s(",sfname);//输出正要执行的函数名
	for(i=0;i<pnum;i++)//输出参数
	{
		if(db_dev.parmtype&(1<<i))//参数是字符串
		{
			DEBUG("%c",'"');			 
			DEBUG("%s",db_dev.parm+db_get_parmpos(i));
			DEBUG("%c",'"');
			temp[i]=(u32)&(db_dev.parm[db_get_parmpos(i)]);
		}else						  //参数是数字
		{
			temp[i]=*(u32*)(db_dev.parm+db_get_parmpos(i));
			if(db_dev.sptype==SP_TYPE_DEC)DEBUG("%lu",temp[i]);//10进制参数显示
			else DEBUG("0X%X",temp[i]);//16进制参数显示 	   
		}
		if(i!=pnum-1)DEBUG(",");
	}
	DEBUG(")");

	switch(db_dev.pnum)
	{
		case 0://无参数(void类型)											  
			res=(*(u32(*)())db_dev.funs[id].func)();
			break;
	    case 1://有1个参数
			res=(*(u32(*)())db_dev.funs[id].func)(temp[0]);
			break;
	    case 2://有2个参数
			res=(*(u32(*)())db_dev.funs[id].func)(temp[0],temp[1]);
			break;
	    case 3://有3个参数
			res=(*(u32(*)())db_dev.funs[id].func)(temp[0],temp[1],temp[2]);
			break;
	    case 4://有4个参数
			res=(*(u32(*)())db_dev.funs[id].func)(temp[0],temp[1],temp[2],temp[3]);
			break;
	    case 5://有5个参数
			res=(*(u32(*)())db_dev.funs[id].func)(temp[0],temp[1],temp[2],temp[3],temp[4]);
			break;
	    case 6://有6个参数
			res=(*(u32(*)())db_dev.funs[id].func)(temp[0],temp[1],temp[2],temp[3],temp[4],\
			temp[5]);
			break;
	}
	
	if(rval==1)//需要返回值.
	{
		if(db_dev.sptype==SP_TYPE_DEC)DEBUG("=%lu;\r\n",res);//输出执行结果(10进制参数显示)
		else DEBUG("=0X%X;\r\n",res);//输出执行结果(16进制参数显示)	   
	}else DEBUG(";\r\n");		//不需要返回值,直接输出结束
	if(db_dev.runtimeflag)	//需要显示函数执行时间
	{ 
		DEBUG("Function Run Time:%d.%1dms\r\n",db_dev.runtime/10,db_dev.runtime%10);//打印函数执行时间 
	}	
}
//db扫描函数
//通过调用该函数,实现db的各个控制.该函数需要每隔一定时间被调用一次
//以及时执行从串口发过来的各个函数.
//本函数可以在中断里面调用,从而实现自动管理.
//如果非ALIENTEK用户,则USART_RX_STA和USART_RX_BUF[]需要用户自己实现
void db_scan(u8 *str)
{
	u8 sta,i;//len,i;  
	u8 *p;
	p = str;
	if((*p > 'z')|| ( *p < ' '))
		return;//非有效字符开头
	//**************适用于NRF24L01，单次最大字节发送长度为32
	for(i=0;i<32;i++)
	{
		if((*p == '\0') || (*p == ')'))
			break;
		p++;
	}
	if((i==0)||(i==32))
		return;//字符串错误
	//**************适用于NRF24L01，单次最大字节发送长度为32
	*(++p) =  '\0';	//在末尾加入结束符.
	
	sta=db_dev.cmd_rec(str);//得到函数各个信息
	if(sta==0)
		db_dev.exe();	//执行函数 
	else
	{
				switch(sta)
				{
					case DB_FUNCERR:
						DEBUG("函数错误!\r\n");   			
						break;	
					case DB_PARMERR:
						DEBUG("参数错误!\r\n");   			
						break;				
					case DB_PARMOVER:
						DEBUG("参数太多!\r\n");   			
						break;		
					case DB_NOFUNCFIND:
						DEBUG("未找到匹配的函数!\r\n");   			
						break;		
				}
	}	
	
}















