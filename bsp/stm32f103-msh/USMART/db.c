#include "db.h"
#include <rtthread.h>

//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com 
//�汾��V3.1
//��Ȩ���У�����ؾ���
//Copyright(C) ����ԭ�� 2011-2021
//All rights reserved
//********************************************************************************
//����˵��
//V1.4
//�����˶Բ���Ϊstring���͵ĺ�����֧��.���÷�Χ������.
//�Ż����ڴ�ռ��,��̬�ڴ�ռ��Ϊ79���ֽ�@10������.��̬��Ӧ���ּ��ַ�������
//V2.0 
//1,�޸���listָ��,��ӡ�������������ʽ.
//2,������idָ��,��ӡÿ����������ڵ�ַ.
//3,�޸��˲���ƥ��,֧�ֺ��������ĵ���(������ڵ�ַ).
//4,�����˺��������Ⱥ궨��.	
//V2.1 20110707		 
//1,����dec,hex����ָ��,�������ò�����ʾ����,��ִ�н���ת��.
//ע:��dec,hex����������ʱ��,���趨��ʾ��������.�����������ʱ��,��ִ�н���ת��.
//��:"dec 0XFF" ��Ὣ0XFFתΪ255,�ɴ��ڷ���.
//��:"hex 100" 	��Ὣ100תΪ0X64,�ɴ��ڷ���
//2,����db_get_cmdname����,���ڻ�ȡָ������.
//V2.2 20110726	
//1,������void���Ͳ����Ĳ���ͳ�ƴ���.
//2,�޸�������ʾ��ʽĬ��Ϊ16����.
//V2.3 20110815
//1,ȥ���˺�����������"("������.
//2,�������ַ��������в�����"("��bug.
//3,�޸��˺���Ĭ����ʾ������ʽ���޸ķ�ʽ. 
//V2.4 20110905
//1,�޸���db_get_cmdname����,������������������.����������������ʱ����������.
//2,����DB_ENTIM_SCAN�궨��,���������Ƿ�ʹ��TIM��ʱִ��scan����.
//V2.5 20110930
//1,�޸�db_init����Ϊvoid db_init(u8 sysclk),���Ը���ϵͳƵ���Զ��趨ɨ��ʱ��.(�̶�100ms)
//2,ȥ����db_init�����е�uart_init����,���ڳ�ʼ���������ⲿ��ʼ��,�����û����й���.
//V2.6 20111009
//1,������read_addr��write_addr��������.��������������������д�ڲ������ַ(��������Ч��ַ).���ӷ������.
//2,read_addr��write_addr������������ͨ������DB_USE_WRFUNSΪ��ʹ�ܺ͹ر�.
//3,�޸���db_strcmp,ʹ��淶��.			  
//V2.7 20111024
//1,�����˷���ֵ16������ʾʱ�����е�bug.
//2,�����˺����Ƿ��з���ֵ���ж�,���û�з���ֵ,�򲻻���ʾ.�з���ֵʱ����ʾ�䷵��ֵ.
//V2.8 20111116
//1,������list�Ȳ���������ָ��ͺ���ܵ���������bug.
//V2.9 20120917
//1,�޸������磺void*xxx(void)���ͺ�������ʶ���bug��
//V3.0 20130425
//1,�������ַ���������ת�����֧�֡�
//V3.1 20131120
//1,����runtimeϵͳָ��,��������ͳ�ƺ���ִ��ʱ��.
//�÷�:
//����:runtime 1 ,��������ִ��ʱ��ͳ�ƹ���
//����:runtime 0 ,��رպ���ִ��ʱ��ͳ�ƹ���
///runtimeͳ�ƹ���,��������:DB_ENTIMX_SCAN Ϊ1,�ſ���ʹ��!!
/////////////////////////////////////////////////////////////////////////////////////
//DB��Դռ�����@MDK 3.80A@2.0�汾��
//FLASH:4K~K�ֽ�(ͨ��DB_USE_HELP��DB_USE_WRFUNS����)
//SRAM:72�ֽ�(���ٵ������)
//SRAM���㹫ʽ:   SRAM=PARM_LEN+72-4  ����PARM_LEN������ڵ���4.
//Ӧ�ñ�֤��ջ��С��100���ֽ�.
////////////////////////////////////////////�û����ò���////////////////////////////////////////////////////	  

#define __DEBUG
#ifdef __DEBUG
#define DEBUG(format, ...) rt_kprintf(format, ##__VA_ARGS__)
#else
#define DEBUG(format, ...)
#endif	    

////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////
//��ʼ�����ڿ�����
//sysclk:ϵͳʱ�ӣ�Mhz��
void db_init(u8 sysclk)
{

	db_dev.sptype=1;	//ʮ��������ʾ����
}		
//��str�л�ȡ������,id,��������Ϣ
//*str:�ַ���ָ��.
//����ֵ:0,ʶ��ɹ�;����,�������.
u8 db_cmd_rec(u8*str) 
{
	u8 sta,i,rval;//״̬	 
	u8 rpnum,spnum;
	u8 rfname[MAX_DB_FNAME_LEN];//�ݴ�ռ�,���ڴ�Ž��յ��ĺ�����  
	u8 sfname[MAX_DB_FNAME_LEN];//��ű��غ�����
	sta=db_get_fname(str,rfname,&rpnum,&rval);//�õ����յ������ݵĺ���������������	  
	if(sta)return sta;//����
	for(i=0;i<db_dev.fnum;i++)
	{
		sta=db_get_fname((u8*)db_dev.funs[i].name,sfname,&spnum,&rval);//�õ����غ���������������
		if(sta)return sta;//���ؽ�������	  
		if(db_strcmp(sfname,rfname)==0)//���
		{
			if(spnum>rpnum)return DB_PARMERR;//��������(���������Դ����������)
			db_dev.id=i;//��¼����ID.
			break;//����.
		}	
	}
	if(i==db_dev.fnum)return DB_NOFUNCFIND;	//δ�ҵ�ƥ��ĺ���
 	sta=db_get_fparam(str,&i);					//�õ�������������	
	if(sta)return sta;								//���ش���
	db_dev.pnum=i;								//����������¼
    return DB_OK;
}
//usamrtִ�к���
//�ú�����������ִ�дӴ����յ�����Ч����.
//���֧��10�������ĺ���,����Ĳ���֧��Ҳ������ʵ��.�����õĺ���.һ��5�����ҵĲ����ĺ����Ѿ����ټ���.
//�ú������ڴ��ڴ�ӡִ�����.��:"������(����1������2...����N)=����ֵ".����ʽ��ӡ.
//����ִ�еĺ���û�з���ֵ��ʱ��,����ӡ�ķ���ֵ��һ�������������.
void db_exe(void)
{
	u8 id,i;
	u32 res=0;		   
	u32 temp[MAX_DB_PARM_NO];//����ת��,ʹ֧֮�����ַ��� 
	u8 sfname[MAX_DB_FNAME_LEN];//��ű��غ�����
	u8 pnum,rval;
	id=db_dev.id;
	if(id>=db_dev.fnum)return;//��ִ��.
	db_get_fname((u8*)db_dev.funs[id].name,sfname,&pnum,&rval);//�õ����غ�����,���������� 
	DEBUG("\r\n%s(",sfname);//�����Ҫִ�еĺ�����
	for(i=0;i<pnum;i++)//�������
	{
		if(db_dev.parmtype&(1<<i))//�������ַ���
		{
			DEBUG("%c",'"');			 
			DEBUG("%s",db_dev.parm+db_get_parmpos(i));
			DEBUG("%c",'"');
			temp[i]=(u32)&(db_dev.parm[db_get_parmpos(i)]);
		}else						  //����������
		{
			temp[i]=*(u32*)(db_dev.parm+db_get_parmpos(i));
			if(db_dev.sptype==SP_TYPE_DEC)DEBUG("%lu",temp[i]);//10���Ʋ�����ʾ
			else DEBUG("0X%X",temp[i]);//16���Ʋ�����ʾ 	   
		}
		if(i!=pnum-1)DEBUG(",");
	}
	DEBUG(")");

	switch(db_dev.pnum)
	{
		case 0://�޲���(void����)											  
			res=(*(u32(*)())db_dev.funs[id].func)();
			break;
	    case 1://��1������
			res=(*(u32(*)())db_dev.funs[id].func)(temp[0]);
			break;
	    case 2://��2������
			res=(*(u32(*)())db_dev.funs[id].func)(temp[0],temp[1]);
			break;
	    case 3://��3������
			res=(*(u32(*)())db_dev.funs[id].func)(temp[0],temp[1],temp[2]);
			break;
	    case 4://��4������
			res=(*(u32(*)())db_dev.funs[id].func)(temp[0],temp[1],temp[2],temp[3]);
			break;
	    case 5://��5������
			res=(*(u32(*)())db_dev.funs[id].func)(temp[0],temp[1],temp[2],temp[3],temp[4]);
			break;
	    case 6://��6������
			res=(*(u32(*)())db_dev.funs[id].func)(temp[0],temp[1],temp[2],temp[3],temp[4],\
			temp[5]);
			break;
	}
	
	if(rval==1)//��Ҫ����ֵ.
	{
		if(db_dev.sptype==SP_TYPE_DEC)DEBUG("=%lu;\r\n",res);//���ִ�н��(10���Ʋ�����ʾ)
		else DEBUG("=0X%X;\r\n",res);//���ִ�н��(16���Ʋ�����ʾ)	   
	}else DEBUG(";\r\n");		//����Ҫ����ֵ,ֱ���������
	if(db_dev.runtimeflag)	//��Ҫ��ʾ����ִ��ʱ��
	{ 
		DEBUG("Function Run Time:%d.%1dms\r\n",db_dev.runtime/10,db_dev.runtime%10);//��ӡ����ִ��ʱ�� 
	}	
}
//dbɨ�躯��
//ͨ�����øú���,ʵ��db�ĸ�������.�ú�����Ҫÿ��һ��ʱ�䱻����һ��
//�Լ�ʱִ�дӴ��ڷ������ĸ�������.
//�������������ж��������,�Ӷ�ʵ���Զ�����.
//�����ALIENTEK�û�,��USART_RX_STA��USART_RX_BUF[]��Ҫ�û��Լ�ʵ��
void db_scan(u8 *str)
{
	u8 sta,i;//len,i;  
	u8 *p;
	p = str;
	if((*p > 'z')|| ( *p < ' '))
		return;//����Ч�ַ���ͷ
	//**************������NRF24L01����������ֽڷ��ͳ���Ϊ32
	for(i=0;i<32;i++)
	{
		if((*p == '\0') || (*p == ')'))
			break;
		p++;
	}
	if((i==0)||(i==32))
		return;//�ַ�������
	//**************������NRF24L01����������ֽڷ��ͳ���Ϊ32
	*(++p) =  '\0';	//��ĩβ���������.
	
	sta=db_dev.cmd_rec(str);//�õ�����������Ϣ
	if(sta==0)
		db_dev.exe();	//ִ�к��� 
	else
	{
				switch(sta)
				{
					case DB_FUNCERR:
						DEBUG("��������!\r\n");   			
						break;	
					case DB_PARMERR:
						DEBUG("��������!\r\n");   			
						break;				
					case DB_PARMOVER:
						DEBUG("����̫��!\r\n");   			
						break;		
					case DB_NOFUNCFIND:
						DEBUG("δ�ҵ�ƥ��ĺ���!\r\n");   			
						break;		
				}
	}	
	
}















