/********************************************************************************/
//////////////////////////////////////////////////////////////////////////////////	
//按键扫描及处理代码
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//STM32开发板
//CSY
//日期:2020/4/09
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 
//All rights reserved									   
//////////////////////////////////////////////////////////////////////////////////
/********************************************************************************/
//修改说明 
//V1.0	
////////////////////////////////////////////////////////////////////////////////// 
/********************************************************************************/
#include "button.h"
#include "stm32f1xx_hal.h"
#include <rthw.h>
#include <rtthread.h>
#include <rtdebug.h>

#include "main_control.h"


//#define BM1  GPIO_PIN_0	//GET_PIN(B, 5)
//#define BM2  GPIO_PIN_1		//GET_PIN(B, 4)
//#define BM3  GPIO_PIN_2		//GET_PIN(B, 3)
//#define BM4  GPIO_PIN_3
//#define BM_PORT	GPIOC

#define BUTTON_THREAD_PRIORITY         8
#define BUTTON_THREAD_STACK_SIZE       256
#define BUTTON_THREAD_TIMESLICE        5

ALIGN(RT_ALIGN_SIZE)
static char thd_btn_stack[BUTTON_THREAD_STACK_SIZE];
static struct rt_thread thd_btn;

button_type  buttons[BUTTON_NUMBER];
uint16_t old_button_status;
uint16_t button_check_on_flag;

uint8_t Channels_RIR[CHANNEL_RIR_MAX];

const unsigned char button_table[BUTTON_NUMBER] =
{
	// Group 0					// Col
	BUTTON_1,				// 0			pow
	BUTTON_2,				// 1		mode
	BUTTON_3,				// 2		minus
	BUTTON_4,					// 3		add
	BUTTON_5,				// 4		mode2
	BUTTON_6,				// 5		mode3
	BUTTON_DUMMY,					// 6
	BUTTON_DUMMY,				//7
	
	BUTTON_RIR1,
	BUTTON_RIR2,
	BUTTON_RIR3,
	BUTTON_RIR4,
	BUTTON_RIR5,
	BUTTON_RIR6,
	BUTTON_DUMMY,
	BUTTON_DUMMY,
	
};

void (*process[BUTTON_NUMBER])( unsigned char flag ) =
{
	button_1,		
	button_2,		
	button_3,		
	button_4,		
	button_5,			
	button_6,			
	button_dummy,			
	button_dummy,						/* dummy button */
	
	bt_rir_1,
	bt_rir_2,
	bt_rir_3,
	bt_rir_4,
	bt_rir_5,
	bt_rir_6,
	button_dummy,			
	button_dummy,						/* dummy button */
};

static int Button_IO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	__HAL_RCC_GPIOB_CLK_ENABLE();
	
	HAL_GPIO_DeInit(GPIOB,DI_165B_Pin | CLK_165B_Pin| LATCH_165B_Pin);
	GPIO_InitStruct.Pin   =  CLK_165B_Pin| LATCH_165B_Pin;
	GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull  = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin   = DI_165B_Pin;
	GPIO_InitStruct.Mode  = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull  = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	return 0;
}
INIT_BOARD_EXPORT(Button_IO_Init);

static int Button_IO_Init_GP2(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	
	HAL_GPIO_DeInit(GPIOA,DI_RIR_Pin);
	HAL_GPIO_DeInit(GPIOC,CLK_RIR_Pin);
	HAL_GPIO_DeInit(GPIOD,LATCH_RIR_Pin);
	
	GPIO_InitStruct.Pin   = LATCH_RIR_Pin;
	GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull  = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin   = CLK_RIR_Pin;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin   = DI_RIR_Pin;
	GPIO_InitStruct.Mode  = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull  = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
		
	return 0;
}
INIT_BOARD_EXPORT(Button_IO_Init_GP2);

//获取一个有效按键值
//uint8_t get_button(void)
//{
//	uint16_t temp;
//	temp = (HAL_GPIO_ReadPin(BM_PORT,BM1)) | ((HAL_GPIO_ReadPin(BM_PORT,BM2))<<1) | ((HAL_GPIO_ReadPin(BM_PORT,BM3))<<2) | ((HAL_GPIO_ReadPin(BM_PORT,BM4))<<3)| 0xFFF0;
//	return (uint8_t)temp;
//}

uint8_t get_165_data_gp2(void)
{
	uint8_t i, byte;
	HAL_GPIO_WritePin(LATCH_RIR_GPIO_Port,LATCH_RIR_Pin,GPIO_PIN_RESET);
	rt_hw_us_delay(100);
	HAL_GPIO_WritePin(LATCH_RIR_GPIO_Port,LATCH_RIR_Pin,GPIO_PIN_SET);
	// init byte
	byte = 0;
	// shift out at HC165_DI
	for( i=0; i<8; i++ )
	{
		HAL_GPIO_WritePin(CLK_RIR_GPIO_Port,CLK_RIR_Pin,GPIO_PIN_RESET);
		rt_hw_us_delay(100);
		// set byte
		if(HAL_GPIO_ReadPin(DI_RIR_GPIO_Port,DI_RIR_Pin)==GPIO_PIN_SET)
			byte |= (0x80 >> i);
		else
			byte = byte;
		HAL_GPIO_WritePin(CLK_RIR_GPIO_Port,CLK_RIR_Pin,GPIO_PIN_SET);
		rt_hw_us_delay(100);
	}
	HAL_GPIO_WritePin(LATCH_RIR_GPIO_Port,LATCH_RIR_Pin,GPIO_PIN_RESET);
	return byte;
}

uint8_t get_165_data(void)
{
	uint8_t i, byte;
	HAL_GPIO_WritePin(GPIOB,LATCH_165B_Pin,GPIO_PIN_RESET);
	rt_hw_us_delay(100);
	HAL_GPIO_WritePin(GPIOB,LATCH_165B_Pin,GPIO_PIN_SET);
	// init byte
	byte = 0;
	// shift out at HC165_DI
	for( i=0; i<8; i++ )
	{
		HAL_GPIO_WritePin(GPIOB,CLK_165B_Pin,GPIO_PIN_RESET);
		rt_hw_us_delay(100);
		// set byte
		if(HAL_GPIO_ReadPin(GPIOB,DI_165B_Pin)==GPIO_PIN_SET)
			byte |= (0x80 >> i);
		else
			byte = byte;
		HAL_GPIO_WritePin(GPIOB,CLK_165B_Pin,GPIO_PIN_SET);
		rt_hw_us_delay(100);
	}
	HAL_GPIO_WritePin(GPIOB,LATCH_165B_Pin,GPIO_PIN_RESET);
	return byte;
}

void button_trigger( unsigned char button_name, unsigned char flag )
{
	/* if disable, do nothing */
	if( !(buttons[button_name].status & BUTTON_ENABLE) )
	{
		/* process button down */
		if( flag == BUTTON_DOWN )
		{
			/* show disable button name */
			//show_value( DISABLE );
		}
		/* process button up */
		else
		{
			//show_domain_value();
		}

		return;
	}
	
	/* process button down */
		if( flag == BUTTON_DOWN )
		{
			/* set pressed flag */
			buttons[button_name].status |= BUTTON_PRESSED;
			buttons[button_name].count = 0;
		}
	/* process button up */
		else
		{
			/* set not pressed flag */
			buttons[button_name].status &= 0xEF;	/* 0xFF ^ BUTTON_PRESSED */
			buttons[button_name].count = 0;
			/* start switch to default domain count */
			if( buttons[button_name].status & BUTTON_DOMAIN_CHANGE )
			{
				//start_switch_default_domain_count();
			}
		}
		
		/* do the button's individual process */
		process[button_name]( flag );
}

extern union_status curr_status;//当前状态
//--------------------------------- button_1
void button_1( unsigned char flag )
{
	RT_DEBUG_LOG(BUTTON_DEBUG,("button_1 %d!\r\n",flag));
	if( flag == BUTTON_DOWN )
	{
			if(curr_status.value.pow_fg==OFF)
				Event_Power();
	}
	#ifdef BUTTON_LONG_PRESS
	else if(flag == BUTTON_LONG_PRESSED)
	{
			Event_Power();
	}
	else if(flag == BUTTON_LONG_RELEASE)
	{

	}
	#endif
	else
	{
		
	}
		
}//---------------------------------
extern union_status curr_status;//当前状态
//---------------------------------button_2
void button_2( unsigned char flag )
{
	RT_DEBUG_LOG(BUTTON_DEBUG,("button_2 %d!\r\n",flag));
	if( flag == BUTTON_DOWN )
	{
		if(curr_status.value.pow_fg==ON)
		{
			curr_status.value.mode = MODE_LUM;
			Event_Mode();
		}
	}
	#ifdef BUTTON_LONG_PRESS
	else if(flag == BUTTON_LONG_PRESSED)
	{

	}
	else if(flag == BUTTON_LONG_RELEASE)
	{

	}
	#endif
	else
	{
		
	}
}
//---------------------------------button_3
void button_3( unsigned char flag )
{
	RT_DEBUG_LOG(BUTTON_DEBUG,("button_3 %d!\r\n",flag));
	if( flag == BUTTON_DOWN )
	{
		Event_Minus();
	}
	#ifdef BUTTON_LONG_PRESS
	else if(flag == BUTTON_LONG_PRESSED)
	{

	}
	else if(flag == BUTTON_LONG_RELEASE)
	{

	}
	#endif
	else
	{
		
	}

}
//---------------------------------button_4
void button_4( unsigned char flag )
{
	RT_DEBUG_LOG(BUTTON_DEBUG,("button_4 %d!\r\n",flag));
	if( flag == BUTTON_DOWN )
	{
		Event_Add();
	}
	#ifdef BUTTON_LONG_PRESS
	else if(flag == BUTTON_LONG_PRESSED)
	{

	}
	else if(flag == BUTTON_LONG_RELEASE)
	{

	}
	#endif
	else
	{
		
	}
}
//---------------------------------button_sewen
void button_5( unsigned char flag )
{
	RT_DEBUG_LOG(BUTTON_DEBUG,("button_5 %d!\r\n",flag));
	if( flag == BUTTON_DOWN )
	{
		if(curr_status.value.pow_fg==ON)
		{
			curr_status.value.mode = MODE_QJ ;
			Event_Mode();
		}
	}
	#ifdef BUTTON_LONG_PRESS
	else if(flag == BUTTON_LONG_PRESSED)
	{

	}
	else if(flag == BUTTON_LONG_RELEASE)
	{

	}
	#endif
	else
	{
		
	}
}//---------------------------------
//---------------------------------button_camera
void button_6( unsigned char flag )
{
  RT_DEBUG_LOG(BUTTON_DEBUG,("button_6 %d!\r\n",flag));
	if( flag == BUTTON_DOWN )
	{
		if(curr_status.value.pow_fg==ON)
		{
			curr_status.value.mode = MODE_DEPTH;
			Event_Mode();
		}
	}
	#ifdef BUTTON_LONG_PRESS
	else if(flag == BUTTON_LONG_PRESSED)
	{

	}
	else if(flag == BUTTON_LONG_RELEASE)
	{

	}
	#endif
	else
	{
		
	}
}//---------------------------------

void button_7( unsigned char flag )
{
  RT_DEBUG_LOG(BUTTON_DEBUG,("button_7 %d!\r\n",flag));
	if( flag == BUTTON_DOWN )
	{
	
	}
	#ifdef BUTTON_LONG_PRESS
	else if(flag == BUTTON_LONG_PRESSED)
	{

	}
	else if(flag == BUTTON_LONG_RELEASE)
	{

	}
	#endif
	else
	{
		
	}
}//---------------------------------
void button_8( unsigned char flag )
{
  RT_DEBUG_LOG(BUTTON_DEBUG,("button_8 %d!\r\n",flag));
	if( flag == BUTTON_DOWN )
	{
	
	}
	#ifdef BUTTON_LONG_PRESS
	else if(flag == BUTTON_LONG_PRESSED)
	{

	}
	else if(flag == BUTTON_LONG_RELEASE)
	{

	}
	#endif
	else
	{
		
	}
}
//---------------------------------
void button_dummy( unsigned char flag )
{
	if( flag == BUTTON_DOWN )
	{
		rt_kprintf("button_nothing DOWN!\r\n");
	}
}
void bt_rir_1( unsigned char flag )
{
	RT_DEBUG_LOG(BUTTON_DEBUG,("rir1 %d!\r\n",flag));
	if( flag == BUTTON_DOWN )
	{
	
	}
	#ifdef BUTTON_LONG_PRESS
	else if(flag == BUTTON_LONG_PRESSED)
	{//long pressed
		if(Channels_RIR[0] ==0)
		{
			Channels_RIR[0] =1;
			RIR_CTL(1);
		}
	}
	else if(flag == BUTTON_LONG_RELEASE)
	{//long release
		
	}
	#endif
	else
	{
		if(Channels_RIR[0])
		{
			Channels_RIR[0]=0;
			RIR_CTL(-1);
		}
	}	
}
void bt_rir_2( unsigned char flag )
{
	RT_DEBUG_LOG(BUTTON_DEBUG,("rir2 %d!\r\n",flag));
	if( flag == BUTTON_DOWN )
	{
	
	}
	#ifdef BUTTON_LONG_PRESS
	else if(flag == BUTTON_LONG_PRESSED)
	{//long pressed
		if(Channels_RIR[1] ==0)
		{
			Channels_RIR[1]=1;
			RIR_CTL(1);
		}
	}
	else if(flag == BUTTON_LONG_RELEASE)
	{//long release
		
	}
	#endif
	else
	{
		if(Channels_RIR[1])
		{
			Channels_RIR[1] =0;
			RIR_CTL(-1);
		}
	}
}

void bt_rir_3( unsigned char flag )
{
	RT_DEBUG_LOG(BUTTON_DEBUG,("rir3 %d!\r\n",flag));
	if( flag == BUTTON_DOWN )
	{
	
	}
	#ifdef BUTTON_LONG_PRESS
	else if(flag == BUTTON_LONG_PRESSED)
	{//long pressed
		if(Channels_RIR[2] ==0)
		{
			Channels_RIR[2] =1;
			RIR_CTL(1);
		}
	}
	else if(flag == BUTTON_LONG_RELEASE)
	{//long release
		
	}
	#endif
	else
	{
		if(Channels_RIR[2])
		{
			Channels_RIR[2]=0;
			RIR_CTL(-1);
		}
	}
}

void bt_rir_4( unsigned char flag )
{
	RT_DEBUG_LOG(BUTTON_DEBUG,("rir4 %d!\r\n",flag));
	if( flag == BUTTON_DOWN )
	{
	
	}
	#ifdef BUTTON_LONG_PRESS
	else if(flag == BUTTON_LONG_PRESSED)
	{//long pressed
		if(Channels_RIR[3] ==0)
		{
			Channels_RIR[3] =1;
			RIR_CTL(1);
		}
	}
	else if(flag == BUTTON_LONG_RELEASE)
	{//long release
		
	}
	#endif
	else
	{
		if(Channels_RIR[3])
		{
			Channels_RIR[3]=0;
			RIR_CTL(-1);
		}
	}
}

void bt_rir_5( unsigned char flag )
{
	RT_DEBUG_LOG(BUTTON_DEBUG,("rir5 %d!\r\n",flag));
	if( flag == BUTTON_DOWN )
	{
	
	}
	#ifdef BUTTON_LONG_PRESS
	else if(flag == BUTTON_LONG_PRESSED)
	{//long pressed
		if(Channels_RIR[4] ==0)
		{
			Channels_RIR[4] =1;
			RIR_CTL(1);
		}
	}
	else if(flag == BUTTON_LONG_RELEASE)
	{//long release
		
	}
	#endif
	else
	{
		if(Channels_RIR[4])
		{
			Channels_RIR[4]=0;
			RIR_CTL(-1);
		}
	}
}

void bt_rir_6( unsigned char flag )
{
	RT_DEBUG_LOG(BUTTON_DEBUG,("rir6 %d!\r\n",flag));
	if( flag == BUTTON_DOWN )
	{
	
	}
	#ifdef BUTTON_LONG_PRESS
	else if(flag == BUTTON_LONG_PRESSED)
	{//long pressed
		if(Channels_RIR[5] ==0)
		{
			Channels_RIR[5] =1;
			RIR_CTL(1);
		}
	}
	else if(flag == BUTTON_LONG_RELEASE)
	{//long release
		
	}
	#endif
	else
	{
		if(Channels_RIR[5])
		{
			Channels_RIR[5]=0;
			RIR_CTL(-1);
		}
	}
}

void bt_rir_7( unsigned char flag )
{
	RT_DEBUG_LOG(BUTTON_DEBUG,("rir7 %d!\r\n",flag));
	if( flag == BUTTON_DOWN )
	{
	
	}
	#ifdef BUTTON_LONG_PRESS
	else if(flag == BUTTON_LONG_PRESSED)
	{//long pressed
		if(Channels_RIR[6] ==0)
		{
			Channels_RIR[6] =1;
			RIR_CTL(1);
		}
	}
	else if(flag == BUTTON_LONG_RELEASE)
	{//long release
		if(Channels_RIR[6])
		{
			Channels_RIR[6]=0;
			RIR_CTL(-1);
		}
	}
	#endif
	else
	{
		
	}
}

static void button_scan_entey(void *parameter)
{
	uint8_t i;
	static uint16_t new_button_status, mask_bit;
	while(1)
  {
		rt_thread_mdelay(10);
		new_button_status = get_165_data();//get_button();//
		new_button_status  = ((0xFF & get_165_data_gp2())<<8) + new_button_status;
		/* compare old status */
		if( new_button_status != old_button_status )
		{
			
			/* process each bit ( button ) */
			mask_bit = 1;
			for( i=0; i<MAX_KIN_COL; i++ )
			{
				/* normal button process */
				if( (button_check_on_flag & mask_bit ) == 0 )
				{
					/* if 1 -> 0 ? (Button Down) */
					if( (old_button_status & mask_bit) &&
						(!(new_button_status & mask_bit)) )
					{
						button_check_on_flag |= mask_bit;
						//button_trigger( button_table[i], BUTTON_DOWN );
					}
					/* else if 0 -> 1 ? ( Button Up ) */
					if( (!(old_button_status & mask_bit)) &&
						(new_button_status & mask_bit) )
					{
						button_check_on_flag |= mask_bit;
						//button_trigger( button_table[i], BUTTON_UP );
					}
					//rt_kprintf("step1!\r\n");
				}
				/*button check */
				else
				{
					/* if 1 -> 0 ? (Button Down) */
					if( (old_button_status & mask_bit) &&
						(!(new_button_status & mask_bit)) )
					{
						button_trigger( button_table[i], BUTTON_DOWN );
						/* save button status */
						old_button_status &= (~mask_bit);
						//rt_kprintf("step2!\r\n");
					}
					/* else if 0 -> 1 ? ( Button Up ) */
					if( (!(old_button_status & mask_bit)) &&
						(new_button_status & mask_bit) )
					{
						button_trigger( button_table[i], BUTTON_UP );
						/* save button status */
						old_button_status |= mask_bit;
						//rt_kprintf("step3!\r\n");
					}
					button_check_on_flag &= (~mask_bit);
				}
				mask_bit <<= 1;
			}

			/* save button status */
			//old_button_status = new_button_status;
		}
				
		#ifdef BUTTON_LONG_PRESS
		//long press dispose
		else
		{
			for( i= BUTTON_RIR1; i<MAX_KIN_COL; i++ )
			{
				if(buttons[i].status & BUTTON_PRESSED)
				{
					if(buttons[i].count < BUTTON_LONG_TIMER )	//40*50ms = 2000ms
					{	
						buttons[i].count++;
						if(buttons[i].count == BUTTON_LONG_TIMER)
						{	process[button_table[i]](BUTTON_LONG_PRESSED);
							buttons[i].count=0;//--;
						}
					}
				}
//				else
//				{
//					if(buttons[i].count < BUTTON_LONG_TIMER )	//10*50ms = 500ms
//					{	
//						buttons[i].count++;
//						if(buttons[i].count == BUTTON_LONG_TIMER)
//							process[button_table[i]](BUTTON_LONG_RELEASE);
//					}
//				}
			}
			
			if( (buttons[BUTTON_1].status & BUTTON_PRESSED) && (curr_status.value.pow_fg==ON) )
			{
					if(buttons[BUTTON_1].count < 150 )	//3000ms
					{	
						buttons[BUTTON_1].count++;
						if(buttons[BUTTON_1].count >= 150)
						{
							process[BUTTON_1](BUTTON_LONG_PRESSED);
							buttons[BUTTON_1].count=0;//--;
						}
					}
			}
		}
		#endif
	}
}

int thread_button_init(void)
{
	uint8_t i;
//	Button_IO_Init();
//	Button_IO_Init_GP2();
	
	for( i=0; i<BUTTON_NUMBER; i++ )
	{
		buttons[i].status = 0x20;
		
		buttons[i].count = BUTTON_LONG_TIMER;
	}
	old_button_status = 0xFF & get_165_data();//get_button();////0xFF & get_165_data();
	old_button_status = 0xFF00+old_button_status;//((0xFF & get_165_data_gp2())<<8) + old_button_status;
	button_check_on_flag = 0;
	
    rt_thread_init(&thd_btn,
                   "thd_btn",
                   button_scan_entey,
                   RT_NULL,
                   &thd_btn_stack[0],
                   sizeof(thd_btn_stack),
                   BUTTON_THREAD_PRIORITY - 1, BUTTON_THREAD_TIMESLICE);
    rt_thread_startup(&thd_btn);

    return 0;
}

//MSH_CMD_EXPORT(thread_button_init, thread button dev);




