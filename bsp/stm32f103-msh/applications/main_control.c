/*
 * Copyright (c) 2006-2019, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-04-24     Roy_chen     First edition
 */
#include "main_control.h"
#include "button.h"
#include "menu.h"
#include "flash_eeprom.h"
#include "sm4.h"

//#define RED_COMPENSATE_ENABLE	1
const uint16_t QJ_Min=100,QJ_Max=1000;

rt_sem_t  sem_warning = RT_NULL;

pwm_value_union set_val;//设定值
union_status curr_status;//当前状态

union_ch_value Mode_GP[MODE_NUMBER];	//设定的各模式下参数 普通模式，光深模式，腔镜模式
union_para SYS_PARA;	//系统参数

uint8_t SET_FACTORY_FLAG;	//回复出厂设置标志
uint8_t MY_BUS_ADDR;
const uint16_t LED_table[MAX_LEVEL] ={0x0001,0x0003,0x0007,0x000F,0x001F,0x003F,0x007F,0x00FF};//,0x01FF,0x03FF};
//const uint16_t PWM_table[MAX_LEVEL] ={100,300,600,800,1000,1200,1400,1600,1800,2000};
//uint8_t SOFT_VERSION[] = "V1.1.0";
extern sMenuItemValue MenuIVS[];

extern uint8_t get_165_data(void);
extern void pwm_output_clear(void);//
extern int SM4_Init(void);//初始化
extern uint8_t SM4_Get_Date(void);
//根据curr_status 刷新显示
#if 1 //8 level but 6 LED
const uint8_t LED_table6[MAX_LEVEL] ={0x04,0x0C,0x0C,0x1C,0x3C,0x7C,0x7C,0xFC};

void led_manual_updata(void)
{
	uint8_t led_status;
	led_status = 0;
	if(curr_status.value.pow_fg==ON)
	{
		 led_status |= (0x01);	//power led
		 if(curr_status.value.mode == MODE_QJ)
		 {
			 led_status |= (0x02);	//mode led
			 led_status |= LED_table6[curr_status.value.qj_grade];//level led
		 }
		 else
		 {
		   led_status |=  LED_table6[curr_status.value.lum_grade];//level led
		 }
	}
	led_status ^= 0xFF;	//按位取反
	Led_Set(led_status);
}
#else
void led_manual_updata(void)
{
	uint16_t led_status;
	led_status = 0;
	if(curr_status.value.pow_fg==ON)
	{
		#if 0 //10档触摸按键
		led_status |= (0x01 << 10);	//power led
		//由于更换了模式按键位置，故模式2、3的指示灯需对调
		if(curr_status.value.mode == MODE_LUM)
			led_status |= (0x01 <<  11);	//mode led
		else if(curr_status.value.mode == MODE_DEPTH)
			led_status |= (0x01 <<  13);	//mode led
		else
			led_status |= (0x01 <<  12);	//mode led
		//led_status |= (0x01 << (curr_status.value.mode + 11));	//mode led
		
		if(curr_status.value.error_Flag)
			led_status |= (0x01 << 15);	//warning flag
		#endif
		
		#if 1 //8档微动开关
		led_status |= (0x01 << 8);	//power led
		if(curr_status.value.mode == MODE_QJ)
		{
			led_status |= (0x01 << 9);	//
		}
	  #endif
		//level led
		if(curr_status.value.mode == MODE_LUM)
			led_status |=  LED_table[curr_status.value.lum_grade];
//		else if(curr_status.value.mode == MODE_YYCTL)
//			led_status |=  LED_table[curr_status.value.lum_grade];
		else if(curr_status.value.mode == MODE_DEPTH)
			led_status |=  LED_table[curr_status.value.lum_grade];
		else if(curr_status.value.mode == MODE_QJ)
			led_status |= LED_table[curr_status.value.qj_grade];
		else
			return;
	}

	led_status ^= 0xFFFF;	//按位取反
	Led_Set_16Bit(led_status);
}
#endif
void Led_Blink(void)
{
	static uint16_t blink_count;
	static uint16_t led_blink;
	static uint16_t led_status;
	led_status = 0;
	if(curr_status.value.pow_fg == OFF) 
		return;
//	if(curr_status.value.mode != MODE_LUM) 
//		return;
		
	if(curr_status.value.sys_set & 0x0C)//使能了阴影管理
	{
		if((curr_status.value.rir)&&(curr_status.value.mode == MODE_LUM))
		{//
			led_blink =1;
			if(curr_status.value.rir > 4)
				blink_count += 4;
			else
				blink_count += curr_status.value.rir;
			
			if(blink_count >100){
				blink_count = 0;
				//低8位
				led_manual_updata();
			}
			else if(blink_count >50)	
			{
				led_status |= (0x01 << 10); //power led
				led_status |= LED_table[curr_status.value.lum_grade];
				led_status ^= 0xFFFF;	//按位取反
				Led_Set_16Bit(led_status);
				
			}
			else
			{
			
			}
		}
		else{	
			if(led_blink){
				led_blink =0;
				led_manual_updata();
			}
		}
	}
	else
		led_manual_updata();//定时更新显示
}


//重新计算各通道目标值
void Event_Updata_Set(void)
{
	union_ch_value *tp_lum_p;
	pwm_value_union temp_p;//
	uint16_t i;
	uint32_t tp_pwm;
	
	if(curr_status.value.pow_fg == ON)
	{
		tp_lum_p = &Mode_GP[curr_status.value.mode];//取得当前模式下的最大最小参数
		if(curr_status.value.mode < MODE_QJ )
		{
			if(curr_status.value.lum_grade == 0){//照度最低档处理
				temp_p.value.ch1 = tp_lum_p->value.ch1_min;	//fudeng1
				temp_p.value.ch2 = tp_lum_p->value.ch1_min;	//fudeng2
				temp_p.value.ch3 = tp_lum_p->value.ch1_min;	//fudeng3
				temp_p.value.ch4 = tp_lum_p->value.ch4_min;	//zhudeng
				#ifdef RED_COMPENSATE_ENABLE
					temp_p.value.ch5 = tp_lum_p->value.ch2_min; //red
				#else
					temp_p.value.ch5 = tp_lum_p->value.ch4_min; //red = zhudeng
				#endif
			}	
			else if(curr_status.value.lum_grade == (LUM_GRADE_NUMB-1)){//if(curr_status.lum_grade == (LUM_GRADE_NUMB-1))//照度最高档处理
				temp_p.value.ch1 = tp_lum_p->value.ch1_max;//fudeng1
				temp_p.value.ch2 = tp_lum_p->value.ch1_max;//fudeng2
				temp_p.value.ch3 = tp_lum_p->value.ch1_max;//fudeng3
				temp_p.value.ch4 = tp_lum_p->value.ch4_max;//zhudeng
			#ifdef RED_COMPENSATE_ENABLE
				temp_p.value.ch5 = tp_lum_p->value.ch2_max; //red
			#else
				temp_p.value.ch5 = tp_lum_p->value.ch4_max; //red = zhudeng
			#endif
			}
			else{// if(curr_status.value.lum_grade < (LUM_GRADE_NUMB-1)){//照度中间档处理
				//当前照度计算公式； 照度= 最小值+ 照度等级*（最大值-最小值）/总等级数
				if(tp_lum_p->value.ch1_max > tp_lum_p->value.ch1_min)//
					temp_p.value.ch1 = tp_lum_p->value.ch1_min + curr_status.value.lum_grade*(tp_lum_p->value.ch1_max - tp_lum_p->value.ch1_min)/(LUM_GRADE_NUMB-1);
				else
					temp_p.value.ch1 = tp_lum_p->value.ch1_min;
				
				temp_p.value.ch2 = temp_p.value.ch1;
				temp_p.value.ch3 = temp_p.value.ch1;
				
				/*if(tp_lum_p->value.ch2_max > tp_lum_p->value.ch2_min)
					temp_p.value.ch2 = tp_lum_p->value.ch2_min + curr_status.value.lum_grade*(tp_lum_p->value.ch2_max - tp_lum_p->value.ch2_min)/(LUM_GRADE_NUMB-1);
				else
					temp_p.value.ch2 = tp_lum_p->value.ch2_min;
				if(tp_lum_p->value.ch3_max > tp_lum_p->value.ch3_min)
					temp_p.value.ch3 = tp_lum_p->value.ch3_min + curr_status.value.lum_grade*(tp_lum_p->value.ch3_max - tp_lum_p->value.ch3_min)/(LUM_GRADE_NUMB-1);
				else
					temp_p.value.ch3 = tp_lum_p->value.ch3_min;
				*/
				if(tp_lum_p->value.ch4_max > tp_lum_p->value.ch4_min)
					temp_p.value.ch4 = tp_lum_p->value.ch4_min + curr_status.value.lum_grade*(tp_lum_p->value.ch4_max - tp_lum_p->value.ch4_min)/(LUM_GRADE_NUMB-1);
				else
					temp_p.value.ch4 = tp_lum_p->value.ch4_min;
				
		#ifdef RED_COMPENSATE_ENABLE
				if(tp_lum_p->value.ch2_max > tp_lum_p->value.ch2_min)
					temp_p.value.ch5 = tp_lum_p->value.ch2_min + curr_status.value.lum_grade*(tp_lum_p->value.ch2_max - tp_lum_p->value.ch2_min)/(LUM_GRADE_NUMB-1);
				else
					temp_p.value.ch5 = tp_lum_p->value.ch2_min;
		#else
				temp_p.value.ch5 = temp_p.value.ch4;//red = zhudeng
		#endif
				
			}
						
			//影阴补偿处理
			if((curr_status.value.mode == MODE_LUM)&&(curr_status.value.sys_set & 0x0C))
			{
				//算法3：加固定的百分比 	140klx*1.15=161klx
				if(curr_status.value.rir > 0)
				{
					 tp_pwm = temp_p.value.ch1*SYS_PARA.value.gain_step/100;
					 temp_p.value.ch1 += tp_pwm;
					 if(temp_p.value.ch1 > MenuIVS[8+1].MaxValue)//PWM_MAX_VALUE)
					     temp_p.value.ch1 = MenuIVS[8+1].MaxValue;//小于最大值
					 temp_p.value.ch2 = temp_p.value.ch1;
				   temp_p.value.ch3 = temp_p.value.ch1;
					 
					 tp_pwm = temp_p.value.ch4*SYS_PARA.value.gain_step/100;
					 temp_p.value.ch4 += tp_pwm;
					 if(temp_p.value.ch4 > MenuIVS[8+7].MaxValue)//小于最大值
						 temp_p.value.ch4 = MenuIVS[8+7].MaxValue;
					 
				#ifdef RED_COMPENSATE_ENABLE
					 tp_pwm = temp_p.value.ch5*SYS_PARA.value.gain_step/100;
					 temp_p.value.ch5 += tp_pwm;
					 if(temp_p.value.ch5 > MenuIVS[8+3].MaxValue)//С??????
						 temp_p.value.ch5 = MenuIVS[8+3].MaxValue;
				#else
					 temp_p.value.ch5 = temp_p.value.ch4;//red = zhudeng
				#endif
					 
				}
				
//					//算法2：加指定的百分比 	140klx*1.15=161klx
//				tp_pwm = temp_p.value.ch1*SYS_PARA.value.gain_step*curr_status.value.rir/100;
//				temp_p.value.ch1 += tp_pwm;
//				if(temp_p.value.ch1 > MenuIVS[8+1].MaxValue)//PWM_MAX_VALUE)
//					temp_p.value.ch1 = MenuIVS[8+1].MaxValue;//小于最大值
//				
//				temp_p.value.ch2 = temp_p.value.ch1;
//				temp_p.value.ch3 = temp_p.value.ch1;
//				
//				/*tp_pwm = temp_p.value.ch2*SYS_PARA.value.gain_step*curr_status.value.rir/100;
//				temp_p.value.ch2 += tp_pwm;
//				if(temp_p.value.ch2 > MenuIVS[8+3].MaxValue)//PWM_MAX_VALUE)
//					temp_p.value.ch2 = MenuIVS[8+3].MaxValue;//小于最大值
//				
//				tp_pwm = temp_p.value.ch3*SYS_PARA.value.gain_step*curr_status.value.rir/100;
//				temp_p.value.ch3 += tp_pwm;
//				if(temp_p.value.ch3 > MenuIVS[8+5].MaxValue)//PWM_MAX_VALUE)
//					temp_p.value.ch3 = MenuIVS[8+5].MaxValue;//小于最大值
//				*/
//				
//				tp_pwm = temp_p.value.ch4*SYS_PARA.value.gain_step*curr_status.value.rir/100;
//				temp_p.value.ch4 += tp_pwm;
//				if(temp_p.value.ch4 > MenuIVS[8+7].MaxValue)//小于最大值
//					temp_p.value.ch4 = MenuIVS[8+7].MaxValue;
//				
//				tp_pwm = temp_p.value.ch5*SYS_PARA.value.gain_step*curr_status.value.rir/100;
//				temp_p.value.ch5 += tp_pwm;
//				if(temp_p.value.ch5 > MenuIVS[8+3].MaxValue)//小于最大值
//					temp_p.value.ch5 = MenuIVS[8+3].MaxValue;
			}
		}
		else
		{//重新计算腔镜模式下的设定值
			for(i=0;i<6;i++)
				temp_p.buf[i] = 0;
			if(curr_status.value.qj_grade == 0)//照度最低档处理
				temp_p.value.ch4 = tp_lum_p->value.ch4_min;
			else if(curr_status.value.qj_grade == (QJ_GP_NUMB-1))
				temp_p.value.ch4 = tp_lum_p->value.ch4_max;
			else
			{
				if(tp_lum_p->value.ch4_max > tp_lum_p->value.ch4_min)
					temp_p.value.ch4 = tp_lum_p->value.ch4_min + curr_status.value.qj_grade*(tp_lum_p->value.ch4_max - tp_lum_p->value.ch4_min)/(LUM_GRADE_NUMB-1);
				else
					temp_p.value.ch4 = tp_lum_p->value.ch4_min;
			}
			//temp_p.value.ch4 = QJ_Min + (QJ_Max-QJ_Min)*curr_status.value.qj_grade/9;
		#ifdef RED_COMPENSATE_ENABLE
    #else
			temp_p.value.ch5 = temp_p.value.ch4;//red = zhudeng
		#endif
			
		}
	}
	else //(curr_status.value.pow_fg == OFF)
	{
		for(i=0;i<6;i++)
			temp_p.buf[i] = 0;
		pwm_output_clear();
	}
	
	//刷新目标值
	for(i=0;i<6;i++)
		set_val.buf[i] = temp_p.buf[i];
	
	//刷新手柄状态指示
	led_manual_updata();
	//save status;
	//AT24CXX_WriteOnePage(EEPROM_ARRD2,curr_status.buf);
}

//指定当前的设定值到各通道
void Set_V4(uint16_t ch1,uint16_t ch2,uint16_t ch3,uint16_t ch4)
{
	set_val.value.ch1 = ch1;
	set_val.value.ch2 = ch2;
	set_val.value.ch3 = ch3;
	set_val.value.ch4 = ch4;

}
void Set_Red(uint16_t ch5)
{
	set_val.value.ch5 = ch5;
}
	
	
//各通道当前的设定值加5
void Add_V4(void)
{
	if(set_val.value.ch1 < PARA_MAX)
		set_val.value.ch1 += 5;
	if(set_val.value.ch2 < PARA_MAX)
		set_val.value.ch2 += 5;
	if(set_val.value.ch3 < PARA_MAX)
		set_val.value.ch3 += 5;
	if(set_val.value.ch4 < PARA_MAX)
		set_val.value.ch4 += 5;
#ifdef RED_COMPENSATE_ENABLE
	if(set_val.value.ch5 < PARA_MAX)
		set_val.value.ch5 += 5;
#else
	set_val.value.ch5 = set_val.value.ch4;// red = zhudeng
#endif

}
void Add_CH4(void)
{
	if(set_val.value.ch4 < PARA_MAX)
		set_val.value.ch4 += 5;
#ifdef RED_COMPENSATE_ENABLE
#else
	set_val.value.ch5 = set_val.value.ch4;// red = zhudeng
#endif
}
//各通道当前的设定值加5
void Minus_V4(void)
{
	if(set_val.value.ch1>5)
		set_val.value.ch1 -= 5;
	if(set_val.value.ch2>5)
		set_val.value.ch2 -= 5;
	if(set_val.value.ch3>5)
		set_val.value.ch3 -= 5;
	if(set_val.value.ch4>5)
		set_val.value.ch4 -= 5;
#ifdef RED_COMPENSATE_ENABLE
	if(set_val.value.ch5>5)
		set_val.value.ch5 -= 5;
#else
	set_val.value.ch5 = set_val.value.ch4;// red = zhudeng
#endif
}
void Minus_CH4(void)
{
	if(set_val.value.ch4>5)
		set_val.value.ch4 -= 5;
#ifdef RED_COMPENSATE_ENABLE
#else
	set_val.value.ch5 = set_val.value.ch4;// red = zhudeng
#endif
}
//保存当前的设定值到指定模式下的min值
void Save_To_MIN(uint16_t mode_no)
{
	uint16_t pos,i;
	pos = mode_no*8;
	//for(i=0;i<4;i++)
		//MenuIVS[pos+i*2].ItemValue = set_val.buf[i];
	MenuIVS[pos].ItemValue = set_val.buf[0];//副灯
	MenuIVS[pos+6].ItemValue = set_val.buf[3];//主灯
#ifdef RED_COMPENSATE_ENABLE
	MenuIVS[pos+2].ItemValue = set_val.buf[4];//红色灯
#else
#endif
	backup_data();
	recover_data();
}
//保存当前的设定值到指定模式下的max值
void Save_To_MAX(uint16_t mode_no)
{
	uint16_t pos,i;
	pos = mode_no*8;
	//for(i=0;i<4;i++)
		//MenuIVS[pos+i*2+1].ItemValue = set_val.buf[i];
	MenuIVS[pos+1].ItemValue = set_val.buf[0];//副灯
	MenuIVS[pos+7].ItemValue = set_val.buf[3];//主灯
#ifdef RED_COMPENSATE_ENABLE
	MenuIVS[pos+3].ItemValue = set_val.buf[4];//红色灯
#else
#endif	
	backup_data();
	recover_data();
}
//恢复当前模式的最低档数据
void Recover_To_MIN(uint16_t mode_no)
{
	uint16_t pos,i;
	pos = mode_no*8;
	//for(i=0;i<4;i++)
		//set_val.buf[i] = MenuIVS[pos+i*2].ItemValue;
	set_val.buf[0] = MenuIVS[pos].ItemValue;//副灯1
	set_val.buf[1] = MenuIVS[pos].ItemValue;//副灯2
	set_val.buf[2] = MenuIVS[pos].ItemValue;//副灯3
	set_val.buf[3] = MenuIVS[pos+6].ItemValue;//主灯
#ifdef RED_COMPENSATE_ENABLE
	set_val.buf[4] = MenuIVS[pos+2].ItemValue;//红色灯
#else
	set_val.buf[4] = set_val.buf[3];//红色灯 = 主灯
#endif
}
//恢复当前模式的最高档数据
void Recover_To_MAX(uint16_t mode_no)
{
	uint16_t pos,i;
	pos = mode_no*8;
	//for(i=0;i<4;i++)
		//set_val.buf[i] = MenuIVS[pos+i*2+1].ItemValue;
	set_val.buf[0] = MenuIVS[pos+1].ItemValue;//副灯1
	set_val.buf[1] = MenuIVS[pos+1].ItemValue;//副灯2
	set_val.buf[2] = MenuIVS[pos+1].ItemValue;//副灯3
	set_val.buf[3] = MenuIVS[pos+7].ItemValue;//主灯
#ifdef RED_COMPENSATE_ENABLE
	set_val.buf[4] = MenuIVS[pos+3].ItemValue;//红色灯
#else
	set_val.buf[4] = set_val.buf[3];//红色灯 = 主灯
#endif
}

void Set_Gain(uint16_t data)
{
	SYS_PARA.value.gain_step = data;
}

void Save_Gain(void)
{
	MenuIVS[24].ItemValue = SYS_PARA.value.gain_step;
	backup_data();
	recover_data();
}

	
//电源控制管理
void Event_Power(void)
{
	if(curr_status.value.pow_fg==ON){
		curr_status.value.pow_fg = OFF;
		HAL_GPIO_WritePin(GP1_POW_GPIO_Port, GP1_POW_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(QJ_POW_GPIO_Port, QJ_POW_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(CAM_POW_GPIO_Port, CAM_POW_Pin, GPIO_PIN_RESET);
	}
	else{
		curr_status.value.pow_fg = ON;
		if(curr_status.value.mode == MODE_QJ)
			HAL_GPIO_WritePin(GP1_POW_GPIO_Port, GP1_POW_Pin, GPIO_PIN_RESET);
		else
			HAL_GPIO_WritePin(GP1_POW_GPIO_Port, GP1_POW_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(QJ_POW_GPIO_Port, QJ_POW_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(CAM_POW_GPIO_Port, CAM_POW_Pin, GPIO_PIN_SET);
	}
	Event_Updata_Set();
}
//复位后的状态刷新
void Event_Updata(void)
{
	if(curr_status.value.pow_fg==ON)
		curr_status.value.pow_fg = OFF;
	else
		curr_status.value.pow_fg=ON;
	Event_Power();
}

//模式切换
void Event_Mode(void)
{
	if(curr_status.value.pow_fg==OFF)
		return;
//	curr_status.value.mode++;
////	if(curr_status.value.mode == MODE_YYCTL)
////	{
////		if((curr_status.value.sys_set & 0x08)==0x00)
////			curr_status.value.mode++;
////	}
//	if(curr_status.value.mode > MODE_QJ )
//	{	
//		curr_status.value.mode = MODE_LUM;
//		HAL_GPIO_WritePin(GP1_POW_GPIO_Port, GP1_POW_Pin, GPIO_PIN_SET);
//	}
	if(curr_status.value.mode != MODE_QJ)
			HAL_GPIO_WritePin(GP1_POW_GPIO_Port, GP1_POW_Pin, GPIO_PIN_SET);
	else
		  HAL_GPIO_WritePin(GP1_POW_GPIO_Port, GP1_POW_Pin, GPIO_PIN_RESET);
	Event_Updata_Set();
}


void Event_Add(void)
{
	if(curr_status.value.pow_fg==OFF)
		return;
	if(curr_status.value.mode < MODE_QJ){
		if(curr_status.value.lum_grade < (LUM_GRADE_NUMB-1))
			curr_status.value.lum_grade++;}
//	else if(curr_status.value.mode == MODE_FACULA){
//		if(curr_status.value.facula < (FACULA_GP_NUMB-1))
//			curr_status.value.facula++;}
//	else if(curr_status.value.mode == MODE_SEWEN){
//		if(curr_status.value.sewen < (SEWEN_GP_NUMB-1))
//			curr_status.value.sewen++;}
	else if(curr_status.value.mode == MODE_QJ){
		if(curr_status.value.qj_grade < (QJ_GP_NUMB-1))
			curr_status.value.qj_grade++;}
	else
		return;
	Event_Updata_Set();
}
void Event_Minus(void)
{	
	if(curr_status.value.pow_fg==OFF)
		return;
	if(curr_status.value.mode < MODE_QJ){
		if(curr_status.value.lum_grade)
			curr_status.value.lum_grade--;}
//	else if(curr_status.value.mode == MODE_FACULA){
//		if(curr_status.value.facula)
//			curr_status.value.facula--;}
//	else if(curr_status.value.mode == MODE_SEWEN){
//		if(curr_status.value.sewen)
//			curr_status.value.sewen--;}
	else if(curr_status.value.mode == MODE_QJ){
		if(curr_status.value.qj_grade)
			curr_status.value.qj_grade--;}
	else
		return;
	Event_Updata_Set();
}

MSH_CMD_EXPORT(Event_Power, power pressed);
MSH_CMD_EXPORT(Event_Mode, mode pressed);
MSH_CMD_EXPORT(Event_Minus, minus pressed);
MSH_CMD_EXPORT(Event_Add, add pressed);

//阴影补偿处理
void RIR_CTL(short int rir_c)
{
	//uint16_t tpch,tp_dacvalue;
	short int tp_rir;
	if(curr_status.value.sys_set&0x80)//设置中
		return;
	
	tp_rir = curr_status.value.rir;
	tp_rir += rir_c;
	if(tp_rir < 0)
		tp_rir = 0;
	if(tp_rir>4)
		tp_rir = 4;	
	curr_status.value.rir = (uint8_t)tp_rir;
	
	if(curr_status.value.mode != MODE_LUM)
		return;
	
	Event_Updata_Set();
}

/* ------------------ */
void Set_Factory_Test( void )
{
	unsigned char button_status;
	
	button_status = get_165_data();
	// check if button [POWER] pressed at the reset time, 
	if( button_status == 0xFE )
	{
		SET_FACTORY_FLAG = ON;
	}
	else
	{
		SET_FACTORY_FLAG = OFF;
	}		
	// mask no use bit
	
}
void Clear_Factory_Set( void )
{
	SET_FACTORY_FLAG = OFF;
}
//备份数据到flash
void backup_data(void)
{
	uint16_t i;
	flash_eeprom_buffer[0] = 0x5555;
	MenuItems_Value_Read(&flash_eeprom_buffer[1],0,MENUITEMVALUE_NO);
	for(i=0;i<sizeof(curr_status)/2;i++)
		flash_eeprom_buffer[1+MENUITEMVALUE_NO+i]=curr_status.buf16[i];
	Flash_Eeprom_Write(1+MENUITEMVALUE_NO+i);
}

//从Flash数据到ram
void recover_data(void)
{
	uint16_t i;
	uint16_t *pbuf;
	Flash_Eeprom_Read_HalfWord(FLASH_EEPROM_ADDR1,flash_eeprom_buffer,MENUITEMVALUE_NO+10);
	
	pbuf = &flash_eeprom_buffer[1];
	MenuItems_Value_Write(&flash_eeprom_buffer[1],0,MENUITEMVALUE_NO);
	
	for(i=0;i<8;i++)
		Mode_GP[0].buf16[i] = pbuf[i];
	
	pbuf = &flash_eeprom_buffer[1+8];
	for(i=0;i<8;i++)
		Mode_GP[1].buf16[i] = pbuf[i];
	
	pbuf = &flash_eeprom_buffer[1+16];
	for(i=0;i<8;i++)
		Mode_GP[2].buf16[i] = pbuf[i];
//	
//	pbuf = &flash_eeprom_buffer[1+24];
//	for(i=0;i<8;i++)
//		Mode_GP[3].buf16[i] = pbuf[i];
	
	SYS_PARA.value.gain_step = pbuf[i];
	SYS_PARA.value.gain_max  = pbuf[i+1];
	if(SYS_PARA.value.gain_step > SYS_PARA.value.gain_max)
		SYS_PARA.value.gain_step = SYS_PARA.value.gain_max;
	pbuf = &flash_eeprom_buffer[1+MENUITEMVALUE_NO];
	
	for(i=0;i<sizeof(curr_status)/2;i++)
		curr_status.buf16[i] = pbuf[i];
	curr_status.value.rir = 0;	
	curr_status.value.error_Flag = 0;
	
	if(curr_status.value.lum_grade > LUM_GRADE_NUMB -1)
		curr_status.value.lum_grade  = 0;
	if(curr_status.value.qj_grade > QJ_GP_NUMB -1)
		curr_status.value.qj_grade  = 0;
	if(curr_status.value.mode > MODE_QJ)
		curr_status.value.mode  = 0;
}

void Reset_To_Factory( void )
{
	uint16_t i;
	for(i=0;i<sizeof(curr_status);i++)
		curr_status.buf[i]=0;
	Menu_IVS_Reset_To_Factory();
	
	backup_data();
}
static void POW_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

		__HAL_RCC_GPIOA_CLK_ENABLE();
    HAL_GPIO_WritePin(GPIOA, GP1_POW_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOA, QJ_POW_Pin, GPIO_PIN_RESET);
		
    GPIO_InitStruct.Pin   = QJ_POW_Pin|GP1_POW_Pin|CAM_POW_Pin;
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
		
		HAL_GPIO_WritePin(GP1_POW_GPIO_Port, GP1_POW_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(QJ_POW_GPIO_Port, QJ_POW_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(CAM_POW_GPIO_Port, CAM_POW_Pin, GPIO_PIN_RESET);
}

void Panel_Init(void)
{
	POW_GPIO_Init();
	SM4_Init();//system set init
	//第一次上电判断
	Flash_Eeprom_Read_HalfWord(FLASH_EEPROM_ADDR1,flash_eeprom_buffer,1);
	if(flash_eeprom_buffer[0]!=0x5555)
		SET_FACTORY_FLAG = ON;
	if((SM4_Get_Date()&0x02)==0x00)
		SET_FACTORY_FLAG = ON;
	if(SET_FACTORY_FLAG == ON)
	{//恢复出厂设置
		Reset_To_Factory();
		SET_FACTORY_FLAG = OFF;
	}
	
	//数据恢复
	recover_data();//
	
	curr_status.value.sys_set = SM4_Get_Date();
	if(curr_status.value.sys_set&0x01)
		MY_BUS_ADDR = 0x10;	//母灯
	else
		MY_BUS_ADDR = 0x20;	//子灯
	//刷新目标输出
	Event_Updata();
	
	
}

