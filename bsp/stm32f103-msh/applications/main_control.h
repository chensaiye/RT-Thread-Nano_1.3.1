#ifndef __MAIN_CONTROL_H
#define __MAIN_CONTROL_H	 

#include "stm32f1xx_hal.h"
#include <rtthread.h>
#include <stdio.h>
#include <stdlib.h>

#include "led_595.h"
#include "tof10120.h" 
#include "24cxx.h" 
#include "button.h"
#include "pwm_tim3.h"

//////////////////////////////////////////////////////////////////////////////////	 
//全局的宏定义，预编译 
//freq = 72M/PWM_FREQ
#define PWM_FREQ_20K	3600
#define PWM_FREQ_30K	2400
#define PWM_FREQ_40K	1800
#define PWM_FREQ_50K	1440
#define PWM_FREQ_60K	1200
#define PWM_FREQ_80K	900
#define PWM_FREQ_100K	720

#define PWM_MIN_VALUE	70
#define PWM_MAX_VALUE	PWM_FREQ_20K


typedef enum
{
  OFF = 0U,
  ON = !OFF
}SwitchStatus;

#define MAX_LEVEL 10

#define LUM_GRADE_NUMB 	10		
//#define FACULA_GP_NUMB 	5
//#define SEWEN_GP_NUMB 	5
#define QJ_GP_NUMB 			10

#define GP1_POW_Pin GPIO_PIN_0
#define GP1_POW_GPIO_Port GPIOA
#define QJ_POW_Pin GPIO_PIN_1
#define QJ_POW_GPIO_Port GPIOA

typedef union
{
	struct
	{
		uint16_t ch1;
		uint16_t ch2;
		uint16_t ch3;
		uint16_t ch4;
		uint16_t ch5;
		uint16_t ch6;
	}value;
	uint16_t buf[6];
}pwm_value_union;


typedef union
{
	struct
	{
			uint16_t ch1_min;
			uint16_t ch1_max;
			uint16_t ch2_min;
			uint16_t ch2_max;
			uint16_t ch3_min;
			uint16_t ch3_max;
			uint16_t ch4_min;
			uint16_t ch4_max;
//			uint16_t ch5_min;
//			uint16_t ch5_max;
//			uint16_t ch6_min;
//			uint16_t ch6_max;
	}value;
	uint8_t buf[16];
	uint16_t buf16[8];
}union_ch_value;

typedef union
{
	/* status type */
	struct
	{
		uint16_t gain_step;			
		uint16_t gain_max;
	}value;
	uint16_t buf[2];
}union_para;

typedef union
{
	struct
	{
	uint8_t lum_grade;	//照度等级
	uint8_t facula;		//光斑等级
	uint8_t sewen;			//色温等级
	uint8_t qj_grade;	//腔镜灯等级
	uint8_t mode;			//模式记录
	uint8_t pow_fg;		//开关标志
	uint8_t bk_fg;			//背光标记
	uint8_t rir;			//阴影传感器触发个数
	uint8_t sys_set;		//系统设置
	uint8_t error_Flag;	//
	}value;
	uint8_t buf[10];
	uint16_t buf16[5];
}union_status;

#define MODE_LUM 			0
//#define MODE_YYCTL	 	1
#define MODE_DEPTH	 	1
#define MODE_QJ			 	2
#define MODE_NUMBER		3

//#define MODE_LUM 			0
//#define MODE_FACULA 	1
//#define MODE_SEWEN	 	2
//#define MODE_QJ			 	3


typedef union
{
	/* status type */
	struct
	{
		uint8_t	open;
		uint32_t	count;
		
		//uint32_t	working_times;
	}value;
	uint8_t buf[5];
}union_sys_count;

extern rt_sem_t  sem_warning;
	
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 
void Show_Version(void);
void Run_Test_Mode( void );
void Set_Factory_Test( void );

void Panel_Init(void);//初始化
void Led_Blink(void);
void pwm_buffer(void);

void led_manual_updata(void);

void Event_Updata_Set(void);
void Event_Power(void);
void Event_Mode(void);
void Event_Minus(void);
void Event_Add(void);
void Event_Change(uint8_t key);

void RIR_CTL(short int rir_c);

void backup_data(void);
void recover_data(void);
//debug function
void Set_V4(uint16_t ch1,uint16_t ch2,uint16_t ch3,uint16_t ch4);
void Save_To_MIN(uint16_t mode_no);
void Save_To_MAX(uint16_t mode_no);
void Set_Gain(uint16_t data);
void Save_Gain(void);

#endif
