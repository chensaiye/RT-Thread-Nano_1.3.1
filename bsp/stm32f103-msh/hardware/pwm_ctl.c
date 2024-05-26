#include "pwm_tim3.h" 
#include "pwm_tim4.h"
#include <rtthread.h>
#include "main_control.h"

extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;
extern void Error_Handler(void);
extern pwm_value_union set_val;//?Ú…?

#define PWM_THREAD_PRIORITY         6
#define PWM_THREAD_STACK_SIZE       256
#define PWM_THREAD_TIMESLICE        5

ALIGN(RT_ALIGN_SIZE)
static char thd_pwm_stack[PWM_THREAD_STACK_SIZE];
static struct rt_thread thd_pwm;

pwm_value_union curr_val;//????

void MX_TIM3_PWM_Start(void)
{
	/* Peripheral clock enable */
    __HAL_RCC_TIM3_CLK_ENABLE();
	HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_3);
	HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_4);
	HAL_TIM_Base_Start(&htim3);
  /* USER CODE END TIM3_Init 2 */
}	
void MX_TIM4_PWM_Start(void)
{
	/* Peripheral clock enable */
    __HAL_RCC_TIM4_CLK_ENABLE();
	//HAL_TIM_PWM_Start(&htim4,TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim4,TIM_CHANNEL_2);
	//HAL_TIM_PWM_Start(&htim4,TIM_CHANNEL_3);
	//HAL_TIM_PWM_Start(&htim4,TIM_CHANNEL_4);
	HAL_TIM_Base_Start(&htim4);
  /* USER CODE END TIM4_Init 2 */
}

//pwm??????????
uint8_t TIM3_PWM_Set_Channel(uint8_t ch,uint16_t value)
{
	if(ch==1)
		TIM3->CCR1 = value;
	else if(ch==2)
		TIM3->CCR2 = value;
	else if(ch==3)
		TIM3->CCR3 = value;
	else if(ch==4)
		TIM3->CCR4 = value;
	else
		return 1;
	return 0;
}
uint8_t TIM4_PWM_Set_Channel(uint8_t ch,uint16_t value)
{
	if(ch==1)
		TIM4->CCR1 = value;
	else if(ch==2)
		TIM4->CCR2 = value;
	/*else if(ch==3)
		TIM3->CCR3 = value;
	else if(ch==4)
		TIM3->CCR4 = value;*/
	else
		return 1;
	return 0;
}
//pwm????
void pwm_output(void)
{
	TIM3_PWM_Set_Channel(1,curr_val.value.ch1);
	TIM3_PWM_Set_Channel(2,curr_val.value.ch2);
	TIM3_PWM_Set_Channel(3,curr_val.value.ch3);
	TIM3_PWM_Set_Channel(4,curr_val.value.ch4);
	//TIM4_PWM_Set_Channel(1,curr_val.value.ch5);
	TIM4_PWM_Set_Channel(2,curr_val.value.ch5);
}

//pwm????????
void pwm_output_clear(void)
{
	uint16_t i;
	for(i=0;i<6;i++)
		curr_val.buf[i]=0;
	TIM3_PWM_Set_Channel(1,curr_val.value.ch1);
	TIM3_PWM_Set_Channel(2,curr_val.value.ch2);
	TIM3_PWM_Set_Channel(3,curr_val.value.ch3);
	TIM3_PWM_Set_Channel(4,curr_val.value.ch4);
	//TIM4_PWM_Set_Channel(1,curr_val.value.ch5);
	TIM4_PWM_Set_Channel(2,curr_val.value.ch5);
}

#define PWM_STARTUP_VALUE	(250u)
//????????£
void pwm_buffer(void)
{
	uint16_t i;
	uint16_t deff;
	for(i=0;i<6;i++)
	{
		if(set_val.buf[i]!= curr_val.buf[i])
		{
#if 0
			curr_val.buf[i]=set_val.buf[i];
#endif
#if 1
			if(curr_val.buf[i] < PWM_STARTUP_VALUE)
			{
				 if(set_val.buf[i] < PWM_STARTUP_VALUE)
						curr_val.buf[i]=set_val.buf[i];
				 else
					  curr_val.buf[i]=PWM_STARTUP_VALUE;
			}
			else
			{
				if(curr_val.buf[i]>set_val.buf[i])
				{
					deff = curr_val.buf[i]-set_val.buf[i];
					if(deff>64)
						deff = deff>>6;
					else
						deff = 1;
					curr_val.buf[i]-=deff;//10;
					if((curr_val.buf[i]<set_val.buf[i])||(curr_val.buf[i] >PWM_MAX_VALUE))
						curr_val.buf[i]=set_val.buf[i];
				}
				else
				{
					deff = set_val.buf[i]-curr_val.buf[i];
					if(deff>64)
						deff = deff>>6;
					else
						deff = 1;
					curr_val.buf[i]+=deff;//10;
					
					if(curr_val.buf[i]>set_val.buf[i])
						curr_val.buf[i]=set_val.buf[i];
				}
		  }
#endif
		}
	}
	pwm_output();
}

/**
  * @brief  The application entry point.
  * @retval int
  */
//void usart_ctl(void)
static void pwm_scan_entey(void *parameter)
{
		MX_TIM3_PWM_Start();
		MX_TIM4_PWM_Start();
		curr_val.value.ch1 = 0;
		curr_val.value.ch2 = 0;
		curr_val.value.ch3 = 0;
		curr_val.value.ch4 = 0;
		curr_val.value.ch5 = 0;
		curr_val.value.ch6 = 0;
		while(1)
		{
			pwm_buffer();
			rt_thread_mdelay(20);
		}
}

int thread_pwm_start(void)
{
		rt_thread_init(&thd_pwm,
                   "thd_pwm",
                   pwm_scan_entey,
                   RT_NULL,
                   &thd_pwm_stack[0],
                   sizeof(thd_pwm_stack),
                   PWM_THREAD_PRIORITY - 1, PWM_THREAD_TIMESLICE);
    rt_thread_startup(&thd_pwm);

    return 0;
}

//MSH_CMD_EXPORT(thread_pwm_start, thread pwm start);
