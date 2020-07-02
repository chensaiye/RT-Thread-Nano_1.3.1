#include "pwm_tim3.h" 
#include "pwm_tim4.h"
#include <rtthread.h>
#include "main_control.h"

extern TIM_HandleTypeDef htim3;
extern void Error_Handler(void);
extern pwm_value_union set_val;//设定值

#define PWM_THREAD_PRIORITY         6
#define PWM_THREAD_STACK_SIZE       256
#define PWM_THREAD_TIMESLICE        5

ALIGN(RT_ALIGN_SIZE)
static char thd_pwm_stack[PWM_THREAD_STACK_SIZE];
static struct rt_thread thd_pwm;

pwm_value_union curr_val;//当前值

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


//pwm各通道值设置
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
//pwm值输出
void pwm_output(void)
{
	TIM3_PWM_Set_Channel(1,curr_val.value.ch1);
	TIM3_PWM_Set_Channel(2,curr_val.value.ch2);
	TIM3_PWM_Set_Channel(3,curr_val.value.ch3);
	TIM3_PWM_Set_Channel(4,curr_val.value.ch4);
}
//通道值缓冲变化
void pwm_buffer(void)
{
	uint16_t i;
	for(i=0;i<6;i++)
	{
		if(set_val.buf[i]!= curr_val.buf[i])
		{
			if(curr_val.buf[i]>set_val.buf[i])
			{
				//curr_val.buf[i]--;
				curr_val.buf[i]-=10;
				if((curr_val.buf[i]<set_val.buf[i])||(curr_val.buf[i] >PWM_MAX_VALUE))
					curr_val.buf[i]=set_val.buf[i];
			}
			else
			{
				//curr_val.buf[i]++;
				curr_val.buf[i]+=10;
				if(curr_val.buf[i]>set_val.buf[i])
					curr_val.buf[i]=set_val.buf[i];
			}
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
		curr_val.value.ch1 = 0;
		curr_val.value.ch2 = 0;
		curr_val.value.ch3 = 0;
		curr_val.value.ch4 = 0;
		while(1)
		{
			pwm_buffer();
			rt_thread_mdelay(10);
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
