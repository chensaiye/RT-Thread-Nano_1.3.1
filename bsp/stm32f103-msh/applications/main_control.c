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

const uint16_t QJ_Min=100,QJ_Max=1000;

pwm_value_union set_val;//设定值
union_status curr_status;//当前状态

union_ch_value Mode_GP[MODE_NUMBER];	//设定的各模式下参数
union_para SYS_PARA;	//系统参数

uint8_t SET_FACTORY_FLAG;	//回复出厂设置标志
uint8_t MY_BUS_ADDR;
const uint16_t LED_table[MAX_LEVEL] ={0x0001,0x0003,0x0007,0x000F,0x001F,0x003F,0x007F,0x00FF,0x01FF,0x03FF};
//const uint16_t PWM_table[MAX_LEVEL] ={100,300,600,800,1000,1200,1400,1600,1800,2000};
//uint8_t SOFT_VERSION[] = "V1.1.0";
extern sMenuItemValue MenuIVS[];

extern uint8_t get_165_data(void);
extern void pwm_output_clear(void);//
extern int SM4_Init(void);//初始化
extern uint8_t SM4_Get_Date(void);
//根据curr_status 刷新显示
void led_manual_updata(void)
{
	uint16_t led_status;
	led_status = 0;
	if(curr_status.value.pow_fg==ON)
	{
		led_status |= (0x01 << 10);	//power led
		led_status |= (0x01 << (curr_status.value.mode + 11));	//mode led
		
		//level led
		if(curr_status.value.mode == MODE_LUM)
			led_status |=  LED_table[curr_status.value.lum_grade];
		else if(curr_status.value.mode == MODE_YYCTL)
			led_status |=  LED_table[curr_status.value.lum_grade];
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

void Led_Blink(void)
{
	static uint16_t blink_count;
	static uint16_t led_blink;
	static uint16_t led_status;
	led_status = 0;
	if(curr_status.value.pow_fg == OFF) 
		return;
	if(curr_status.value.mode > MODE_LUM) 
		return;
		
	//if(curr_status.value.sys_set & 0x08)
	{
		if(curr_status.value.rir)
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
				temp_p.value.ch1 = tp_lum_p->value.ch1_min;
				temp_p.value.ch2 = tp_lum_p->value.ch2_min;
				temp_p.value.ch3 = tp_lum_p->value.ch3_min;
				temp_p.value.ch4 = tp_lum_p->value.ch4_min;
			}	
			else if(curr_status.value.lum_grade == (LUM_GRADE_NUMB-1)){//if(curr_status.lum_grade == (LUM_GRADE_NUMB-1))//照度最高档处理
				temp_p.value.ch1 = tp_lum_p->value.ch1_max;
				temp_p.value.ch2 = tp_lum_p->value.ch2_max;
				temp_p.value.ch3 = tp_lum_p->value.ch3_max;
				temp_p.value.ch4 = tp_lum_p->value.ch4_max;
	//			temp_p.value.ch6 = tp_facula_p->value.ch6_max;
	//			temp_p.value.ch7 = tp_facula_p->value.ch7_max;
			}
			else{// if(curr_status.value.lum_grade < (LUM_GRADE_NUMB-1)){//照度中间档处理
				//当前照度计算公式； 照度= 最小值+ 照度等级*（最大值-最小值）/总等级数
				if(tp_lum_p->value.ch1_max > tp_lum_p->value.ch1_min)//
					temp_p.value.ch1 = tp_lum_p->value.ch1_min + curr_status.value.lum_grade*(tp_lum_p->value.ch1_max - tp_lum_p->value.ch1_min)/(LUM_GRADE_NUMB-1);
				else
					temp_p.value.ch1 = tp_lum_p->value.ch1_min;
				if(tp_lum_p->value.ch2_max > tp_lum_p->value.ch2_min)
					temp_p.value.ch2 = tp_lum_p->value.ch2_min + curr_status.value.lum_grade*(tp_lum_p->value.ch2_max - tp_lum_p->value.ch2_min)/(LUM_GRADE_NUMB-1);
				else
					temp_p.value.ch2 = tp_lum_p->value.ch2_min;
				if(tp_lum_p->value.ch3_max > tp_lum_p->value.ch3_min)
					temp_p.value.ch3 = tp_lum_p->value.ch3_min + curr_status.value.lum_grade*(tp_lum_p->value.ch3_max - tp_lum_p->value.ch3_min)/(LUM_GRADE_NUMB-1);
				else
					temp_p.value.ch3 = tp_lum_p->value.ch3_min;
				
				if(tp_lum_p->value.ch4_max > tp_lum_p->value.ch4_min)
					temp_p.value.ch4 = tp_lum_p->value.ch4_min + curr_status.value.lum_grade*(tp_lum_p->value.ch4_max - tp_lum_p->value.ch4_min)/(LUM_GRADE_NUMB-1);
				else
					temp_p.value.ch4 = tp_lum_p->value.ch4_min;
			}
						
			//影阴补偿处理
			if((curr_status.value.mode == MODE_YYCTL))//&&(curr_status.value.sys_set & 0x08))
			{
				
					//算法2：加指定的百分比 	140klx*1.15=161klx
				tp_pwm = temp_p.value.ch1*SYS_PARA.value.gain_step*curr_status.value.rir/100;
				temp_p.value.ch1 += tp_pwm;
				if(temp_p.value.ch1 > MenuIVS[8+1].MaxValue)//PWM_MAX_VALUE)
					temp_p.value.ch1 = MenuIVS[8+1].MaxValue;//小于最大值
				
				tp_pwm = temp_p.value.ch2*SYS_PARA.value.gain_step*curr_status.value.rir/100;
				temp_p.value.ch2 += tp_pwm;
				if(temp_p.value.ch2 > MenuIVS[8+3].MaxValue)//PWM_MAX_VALUE)
					temp_p.value.ch2 = MenuIVS[8+3].MaxValue;//小于最大值
				
				tp_pwm = temp_p.value.ch3*SYS_PARA.value.gain_step*curr_status.value.rir/100;
				temp_p.value.ch3 += tp_pwm;
				if(temp_p.value.ch3 > MenuIVS[8+5].MaxValue)//PWM_MAX_VALUE)
					temp_p.value.ch3 = MenuIVS[8+5].MaxValue;//小于最大值
								
				tp_pwm = temp_p.value.ch4*SYS_PARA.value.gain_step*curr_status.value.rir/100;
				temp_p.value.ch4 += tp_pwm;
				if(temp_p.value.ch4 > MenuIVS[8+7].MaxValue)//小于最大值
					temp_p.value.ch4 = MenuIVS[8+7].MaxValue;
			}
		}
		else
		{//重新计算腔镜模式下的设定值
			for(i=0;i<4;i++)
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
		}
	}
	else //(curr_status.value.pow_fg == OFF)
	{
		for(i=0;i<4;i++)
			temp_p.buf[i] = 0;
		pwm_output_clear();
	}
	
	//刷新目标值
	for(i=0;i<4;i++)
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
//保存当前的设定值到指定模式下的min值
void Save_To_MIN(uint16_t mode_no)
{
	uint16_t pos,i;
	pos = mode_no*8;
	for(i=0;i<4;i++)
		MenuIVS[pos+i*2].ItemValue = set_val.buf[i];
	
	backup_data();
	recover_data();
}
//保存当前的设定值到指定模式下的max值
void Save_To_MAX(uint16_t mode_no)
{
	uint16_t pos,i;
	pos = mode_no*8;
	for(i=0;i<4;i++)
		MenuIVS[pos+i*2+1].ItemValue = set_val.buf[i];
	backup_data();
	recover_data();
}

void Set_Gain(uint16_t data)
{
	SYS_PARA.value.gain_step = data;
}

void Save_Gain(void)
{
	MenuIVS[32].ItemValue = SYS_PARA.value.gain_step;
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
	}
	else{
		curr_status.value.pow_fg = ON;
		if(curr_status.value.mode == MODE_QJ)
			HAL_GPIO_WritePin(GP1_POW_GPIO_Port, GP1_POW_Pin, GPIO_PIN_RESET);
		else
			HAL_GPIO_WritePin(GP1_POW_GPIO_Port, GP1_POW_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(QJ_POW_GPIO_Port, QJ_POW_Pin, GPIO_PIN_SET);
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
	curr_status.value.mode++;
	if(curr_status.value.mode == MODE_YYCTL)
	{
		if((curr_status.value.sys_set & 0x08)==0x00)
			curr_status.value.mode++;
	}
	if(curr_status.value.mode > MODE_QJ )
	{	
		curr_status.value.mode = MODE_LUM;
		HAL_GPIO_WritePin(GP1_POW_GPIO_Port, GP1_POW_Pin, GPIO_PIN_SET);
	}
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
	
	pbuf = &flash_eeprom_buffer[1+24];
	for(i=0;i<8;i++)
		Mode_GP[3].buf16[i] = pbuf[i];
	
	SYS_PARA.value.gain_step = pbuf[i];
	SYS_PARA.value.gain_max  = pbuf[i+1];
	
	pbuf = &flash_eeprom_buffer[1+MENUITEMVALUE_NO];
	
	for(i=0;i<sizeof(curr_status)/2;i++)
		curr_status.buf16[i] = pbuf[i];
	curr_status.value.rir = 0;	
	
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
		
    GPIO_InitStruct.Pin   = QJ_POW_Pin|GP1_POW_Pin;
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
		
		HAL_GPIO_WritePin(GP1_POW_GPIO_Port, GP1_POW_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(QJ_POW_GPIO_Port, QJ_POW_Pin, GPIO_PIN_RESET);
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

