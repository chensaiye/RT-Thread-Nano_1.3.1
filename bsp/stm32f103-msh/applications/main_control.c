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

const uint16_t QJ_Min=100,QJ_Max=1000;

pwm_value_union set_val;//�趨ֵ
union_status curr_status;//��ǰ״̬

union_ch_value Mode_GP[MODE_NUMBER];	//�趨�ĸ�ģʽ�²���
union_para SYS_PARA;	//ϵͳ����

uint8_t SET_FACTORY_FLAG;	//�ظ��������ñ�־

const uint16_t LED_table[MAX_LEVEL] ={0x0001,0x0003,0x0007,0x000F,0x001F,0x003F,0x007F,0x00FF,0x01FF,0x03FF};
//const uint16_t PWM_table[MAX_LEVEL] ={100,300,600,800,1000,1200,1400,1600,1800,2000};
//uint8_t SOFT_VERSION[] = "V1.1.0";
extern sMenuItemValue MenuIVS[];

extern uint8_t get_165_data(void);

//����curr_status ˢ����ʾ
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

	led_status ^= 0xFFFF;	//��λȡ��
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
	if(curr_status.value.mode != MODE_YYCTL) 
		return;
	
	
	//if((SYSTEM_SET == IS_FALSE)&&(POWER_FLAG == ON)&&(CURRENT_MODE == MODE_SHADOW))
	
		if(curr_status.value.rir)
		{//
			led_blink =1;
			if(curr_status.value.rir > 4)
				blink_count += 4;
			else
				blink_count += curr_status.value.rir;
			
			if(blink_count >100){
				blink_count = 0;
				//��8λ
				led_manual_updata();
			}
			else if(blink_count >50)	
			{
				led_status |= (0x01 << 10); //power led
				led_status |= LED_table[curr_status.value.lum_grade];
				led_status ^= 0xFFFF;	//��λȡ��
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


//���¼����ͨ��Ŀ��ֵ
void Event_Updata_Set(void)
{
	union_ch_value *tp_lum_p;
	pwm_value_union temp_p;//
	uint16_t i;
	uint32_t tp_pwm;
	
	if(curr_status.value.pow_fg == ON)
	{
		if(curr_status.value.mode < MODE_QJ )
		{
			//
			tp_lum_p = &Mode_GP[curr_status.value.mode];//ȡ�õ�ǰģʽ�µ������С����
			if(curr_status.value.lum_grade == 0){//�ն���͵�����
				temp_p.value.ch1 = tp_lum_p->value.ch1_min;
				temp_p.value.ch2 = tp_lum_p->value.ch2_min;
				temp_p.value.ch3 = tp_lum_p->value.ch3_min;
				temp_p.value.ch4 = tp_lum_p->value.ch4_min;
			}	
			else if(curr_status.value.lum_grade == (LUM_GRADE_NUMB-1)){//if(curr_status.lum_grade == (LUM_GRADE_NUMB-1))//�ն���ߵ�����
				temp_p.value.ch1 = tp_lum_p->value.ch1_max;
				temp_p.value.ch2 = tp_lum_p->value.ch2_max;
				temp_p.value.ch3 = tp_lum_p->value.ch3_max;
				temp_p.value.ch4 = tp_lum_p->value.ch4_max;
	//			temp_p.value.ch6 = tp_facula_p->value.ch6_max;
	//			temp_p.value.ch7 = tp_facula_p->value.ch7_max;
			}
			else{// if(curr_status.value.lum_grade < (LUM_GRADE_NUMB-1)){//�ն��м䵵����
				//��ǰ�նȼ��㹫ʽ�� �ն�= ��Сֵ+ �նȵȼ�*�����ֵ-��Сֵ��/�ܵȼ���
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
						
			//Ӱ����������
			if(curr_status.value.mode == MODE_YYCTL)
			{
					//�㷨2����ָ���İٷֱ� 	140klx*1.15=161klx
				tp_pwm = temp_p.value.ch1*SYS_PARA.value.gain_step*curr_status.value.rir/100;
				temp_p.value.ch1 += tp_pwm;
				if(temp_p.value.ch1 > PWM_MAX_VALUE)
					temp_p.value.ch1 = PWM_MAX_VALUE;//С�����ֵ
				temp_p.value.ch2 = temp_p.value.ch1;
				temp_p.value.ch3 = temp_p.value.ch1;
				
				tp_pwm = temp_p.value.ch4*SYS_PARA.value.gain_step*curr_status.value.rir/100;
				temp_p.value.ch4 += tp_pwm;
				if(temp_p.value.ch4 > PWM_MAX_VALUE)//С�����ֵ
					temp_p.value.ch4 = PWM_MAX_VALUE;
			}
		}
		else
		{//���¼���ǻ��ģʽ�µ��趨ֵ
			for(i=0;i<4;i++)
				temp_p.buf[i] = 0;
			if(curr_status.value.qj_grade == 0)//�ն���͵�����
				temp_p.value.ch4 = tp_lum_p->value.ch4_min;
			else if(curr_status.value.qj_grade == (QJ_GP_NUMB-1))
				temp_p.value.ch4 = tp_lum_p->value.ch4_max;
			else
			{
				if(tp_lum_p->value.ch4_max > tp_lum_p->value.ch4_min)
					temp_p.value.ch4 = tp_lum_p->value.ch4_min + curr_status.value.lum_grade*(tp_lum_p->value.ch4_max - tp_lum_p->value.ch4_min)/(LUM_GRADE_NUMB-1);
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
	}
	
	//ˢ��Ŀ��ֵ
	for(i=0;i<4;i++)
		set_val.buf[i] = temp_p.buf[i];
	
	//ˢ���ֱ�״ָ̬ʾ
	led_manual_updata();
	//save status;
	//AT24CXX_WriteOnePage(EEPROM_ARRD2,curr_status.buf);
}

//ָ����ǰ���趨ֵ����ͨ��
void Set_V4(uint16_t ch1,uint16_t ch2,uint16_t ch3,uint16_t ch4)
{
	set_val.value.ch1 = ch1;
	set_val.value.ch2 = ch2;
	set_val.value.ch3 = ch3;
	set_val.value.ch4 = ch4;
}
//���浱ǰ���趨ֵ��ָ��ģʽ�µ�minֵ
void Save_To_MIN(uint16_t mode_no)
{
	uint16_t pos,i;
	pos = mode_no*8;
	for(i=0;i<4;i++)
		MenuIVS[pos+i*2].ItemValue = set_val.buf[i];
	
	backup_data();
	recover_data();
}
//���浱ǰ���趨ֵ��ָ��ģʽ�µ�maxֵ
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

	
//��Դ���ƹ���
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
//��λ���״̬ˢ��
void Event_Updata(void)
{
	if(curr_status.value.pow_fg==ON)
		curr_status.value.pow_fg = OFF;
	else
		curr_status.value.pow_fg=ON;
	Event_Power();
}

//ģʽ�л�
void Event_Mode(void)
{
	if(curr_status.value.pow_fg==OFF)
		return;
	curr_status.value.mode++;
	if(curr_status.value.mode > MODE_QJ )
		curr_status.value.mode = MODE_LUM;
	
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


//��Ӱ��������
void RIR_CTL(short int rir_c)
{
	//uint16_t tpch,tp_dacvalue;
	short int tp_rir;
	
	tp_rir = curr_status.value.rir;
	tp_rir += rir_c;
	if(tp_rir < 0)
		tp_rir = 0;
		
	curr_status.value.rir = (uint8_t)tp_rir;
	if(tp_rir>4)
		tp_rir = 4;
	if(curr_status.value.mode != MODE_YYCTL)
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
//�������ݵ�flash
void backup_data(void)
{
	uint16_t i;
	flash_eeprom_buffer[0] = 0x5555;
	MenuItems_Value_Read(&flash_eeprom_buffer[1],0,MENUITEMVALUE_NO);
	for(i=0;i<sizeof(curr_status)/2;i++)
		flash_eeprom_buffer[1+MENUITEMVALUE_NO+i]=curr_status.buf16[i];
	Flash_Eeprom_Write(1+MENUITEMVALUE_NO+i);
}

//��Flash���ݵ�ram
void recover_data(void)
{
	uint16_t i;
	uint16_t *pbuf;
	Flash_Eeprom_Read_HalfWord(FLASH_EEPROM_ADDR1,flash_eeprom_buffer,MENUITEMVALUE_NO+10);
	
	pbuf = &flash_eeprom_buffer[1];
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
	
//	Mode_GP[MODE_LUM].value.ch4_min = pbuf[0];
//	Mode_GP[MODE_LUM].value.ch4_max = pbuf[1];
//	Mode_GP[MODE_LUM].value.ch1_min = pbuf[2];
//	Mode_GP[MODE_LUM].value.ch1_max = pbuf[3];
//	Mode_GP[MODE_LUM].value.ch2_min = pbuf[2];
//	Mode_GP[MODE_LUM].value.ch2_max = pbuf[3];
//	Mode_GP[MODE_LUM].value.ch3_min = pbuf[2];
//	Mode_GP[MODE_LUM].value.ch3_max = pbuf[3];
//	
//	Mode_GP[MODE_YYCTL].value.ch4_min = pbuf[4];
//	Mode_GP[MODE_YYCTL].value.ch4_max = pbuf[5];
//	Mode_GP[MODE_YYCTL].value.ch1_min = pbuf[6];
//	Mode_GP[MODE_YYCTL].value.ch1_max = pbuf[7];
//	Mode_GP[MODE_YYCTL].value.ch2_min = pbuf[6];
//	Mode_GP[MODE_YYCTL].value.ch2_max = pbuf[7];
//	Mode_GP[MODE_YYCTL].value.ch3_min = pbuf[6];
//	Mode_GP[MODE_YYCTL].value.ch3_max = pbuf[7];
//	
//	Mode_GP[MODE_DEPTH].value.ch4_min = pbuf[8];
//	Mode_GP[MODE_DEPTH].value.ch4_max = pbuf[9];
//	Mode_GP[MODE_DEPTH].value.ch1_min = pbuf[10];
//	Mode_GP[MODE_DEPTH].value.ch1_max = pbuf[11];
//	Mode_GP[MODE_DEPTH].value.ch2_min = pbuf[10];
//	Mode_GP[MODE_DEPTH].value.ch2_max = pbuf[11];
//	Mode_GP[MODE_DEPTH].value.ch3_min = pbuf[10];
//	Mode_GP[MODE_DEPTH].value.ch3_max = pbuf[11];
//	
//	Mode_GP[MODE_QJ].value.ch4_min = pbuf[12];
//	Mode_GP[MODE_QJ].value.ch4_max = pbuf[13];
//	Mode_GP[MODE_QJ].value.ch1_min = 0;//flash_eeprom_buffer[14];
//	Mode_GP[MODE_QJ].value.ch1_max = 0;//flash_eeprom_buffer[15];
//	Mode_GP[MODE_QJ].value.ch2_min = 0;//flash_eeprom_buffer[14];
//	Mode_GP[MODE_QJ].value.ch2_max = 0;//flash_eeprom_buffer[15];
//	Mode_GP[MODE_QJ].value.ch3_min = 0;//flash_eeprom_buffer[14];
//	Mode_GP[MODE_QJ].value.ch3_max = 0;//flash_eeprom_buffer[15];
	
//	SYS_PARA.value.gain_step = pbuf[16];
//	SYS_PARA.value.gain_max  = pbuf[17];
	
	pbuf = &flash_eeprom_buffer[1+MENUITEMVALUE_NO];
	for(i=0;i<sizeof(curr_status)/2;i++)
		curr_status.buf16[i] = pbuf[i];
		
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
	//��һ���ϵ��ж�
	Flash_Eeprom_Read_HalfWord(FLASH_EEPROM_ADDR1,flash_eeprom_buffer,1);
	if(flash_eeprom_buffer[0]!=0x5555)
		SET_FACTORY_FLAG = ON;
	if(SET_FACTORY_FLAG == ON)
	{//�ָ���������
		Reset_To_Factory();
		SET_FACTORY_FLAG = OFF;
	}
	
	//���ݻָ�
	recover_data();//
		
	//ˢ��Ŀ�����
	Event_Updata();
	
	
}

