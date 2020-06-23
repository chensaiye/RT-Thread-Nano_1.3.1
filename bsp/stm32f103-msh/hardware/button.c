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

#define HC165_LATCH  GPIO_PIN_5	//GET_PIN(B, 5)
#define HC165_CLK	GPIO_PIN_4		//GET_PIN(B, 4)
#define HC165_DI  GPIO_PIN_3		//GET_PIN(B, 3)
#define HC165_PORT	GPIOB

#define BM1  GPIO_PIN_0	//GET_PIN(B, 5)
#define BM2  GPIO_PIN_1		//GET_PIN(B, 4)
#define BM3  GPIO_PIN_2		//GET_PIN(B, 3)
#define BM4  GPIO_PIN_3
#define BM_PORT	GPIOC

#define BUTTON_THREAD_PRIORITY         8
#define BUTTON_THREAD_STACK_SIZE       256
#define BUTTON_THREAD_TIMESLICE        5

ALIGN(RT_ALIGN_SIZE)
static char thd_btn_stack[BUTTON_THREAD_STACK_SIZE];
static struct rt_thread thd_btn;

button_type  buttons[BUTTON_NUMBER];
uint8_t old_button_status;
uint8_t button_check_on_flag;

const unsigned char button_table[BUTTON_NUMBER] =
{
	// Group 0					// Col
	BUTTON_1,				// 0			pow
	BUTTON_2,				// 1		mode
	BUTTON_3,				// 2		minus
	BUTTON_4,					// 3		add
	BUTTON_DUMMY,				// 4		sewen
	BUTTON_DUMMY,				// 5		camera pow
	BUTTON_DUMMY,					// 6
	BUTTON_DUMMY,				//7
};

void (*process[BUTTON_NUMBER])( unsigned char flag ) =
{
	button_1,		
	button_2,		
	button_3,		
	button_4,		
	button_5,		
	button_6,		
	button_7,		
	button_dummy,						/* dummy button */
};

static int Button_IO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	
	HAL_GPIO_DeInit(HC165_PORT,HC165_LATCH | HC165_CLK| HC165_DI);
	GPIO_InitStruct.Pin   =  HC165_CLK| HC165_LATCH;
	GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull  = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(HC165_PORT, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin   = HC165_DI;
	GPIO_InitStruct.Mode  = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull  = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(HC165_PORT, &GPIO_InitStruct);
		
	GPIO_InitStruct.Pin   = BM1 |BM2 |BM3 |BM4;
	GPIO_InitStruct.Mode  = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull  = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(BM_PORT, &GPIO_InitStruct);
	
	return 0;
}
INIT_BOARD_EXPORT(Button_IO_Init);

//获取一个有效按键值
uint8_t get_button(void)
{
	uint16_t temp;
	temp = (HAL_GPIO_ReadPin(BM_PORT,BM1)) | ((HAL_GPIO_ReadPin(BM_PORT,BM2))<<1) | ((HAL_GPIO_ReadPin(BM_PORT,BM3))<<2) | ((HAL_GPIO_ReadPin(BM_PORT,BM4))<<3)| 0xFFF0;
	return (uint8_t)temp;
}

//读165 数据 按键
//uint8_t get_165_data(void)
//{
//	uint8_t i, byte;
//	rt_pin_write(HC165_LATCH, PIN_LOW);// Parallel-load
//	rt_thread_delay(5);
//	rt_pin_write(HC165_LATCH, PIN_HIGH);		// Parallel-latch
//	// init byte
//	byte = 0;
//	// shift out at HC165_DI
//	for( i=0; i<8; i++ )
//	{
//		rt_pin_write(HC165_CLK, PIN_LOW);		// make HC165_CLK LOW
//		rt_thread_delay(5);
//		// set byte
//		if( rt_pin_read(HC165_DI))
//			byte |= (0x80 >> i);
//		rt_pin_write(HC165_CLK, PIN_HIGH);		// make HC165_CLK HIGH
//		rt_thread_delay(5);
//	}
//	rt_pin_write(HC165_LATCH, PIN_LOW);
//	return byte;
//}
uint8_t get_165_data(void)
{
	uint8_t i, byte;
	HAL_GPIO_WritePin(HC165_PORT,HC165_LATCH,GPIO_PIN_RESET);
	rt_hw_us_delay(100);
	HAL_GPIO_WritePin(HC165_PORT,HC165_LATCH,GPIO_PIN_SET);
	// init byte
	byte = 0;
	// shift out at HC165_DI
	for( i=0; i<8; i++ )
	{
		HAL_GPIO_WritePin(HC165_PORT,HC165_CLK,GPIO_PIN_RESET);
		rt_hw_us_delay(100);
		// set byte
		if(HAL_GPIO_ReadPin(HC165_PORT,HC165_DI)==GPIO_PIN_SET)
			byte |= (0x80 >> i);
		else
			byte = byte;
		HAL_GPIO_WritePin(HC165_PORT,HC165_CLK,GPIO_PIN_SET);
		rt_hw_us_delay(100);
	}
	HAL_GPIO_WritePin(HC165_PORT,HC165_LATCH,GPIO_PIN_RESET);
	
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
//--------------------------------- button_1
void button_1( unsigned char flag )
{
	if( flag == BUTTON_DOWN )
	{
		rt_kprintf("button_1 DOWN!\r\n");
	}
	#ifdef BUTTON_LONG_PRESS
	else if(flag == BUTTON_LONG_PRESSED)
	{
		rt_kprintf("button_3 LONG!\r\n");
	}
	#endif
	else
		rt_kprintf("button_1 UP!\r\n");
}//---------------------------------
//---------------------------------button_2
void button_2( unsigned char flag )
{
	if( flag == BUTTON_DOWN )
	{
		rt_kprintf("button_2 DOWN!\r\n");
	}
	#ifdef BUTTON_LONG_PRESS
	else if(flag == BUTTON_LONG_PRESSED)
	{

	}
	#endif
	else
		rt_kprintf("button_2 UP!\r\n");
}
//---------------------------------button_3
void button_3( unsigned char flag )
{
	if( flag == BUTTON_DOWN )
	{
		rt_kprintf("button_3 dwon!\r\n");
	}
	#ifdef BUTTON_LONG_PRESS
	else if(flag == BUTTON_LONG_PRESSED)
	{
		//rt_kprintf("button_3 LONG!\r\n");
	}
	#endif
	else
	{
		rt_kprintf("button_3 up!\r\n");
	}

}
//---------------------------------button_4
void button_4( unsigned char flag )
{
 if( flag == BUTTON_DOWN )
	{
		rt_kprintf("button_4 DOWN!\r\n");
	}
	else
		rt_kprintf("button_4 UP!\r\n");
}
//---------------------------------button_sewen
void button_5( unsigned char flag )
{
	
   if( flag == BUTTON_DOWN )
	{
		rt_kprintf("button_5 DOWN!\r\n");
	}
	else
		rt_kprintf("button_5 UP!\r\n");
}//---------------------------------
//---------------------------------button_camera
void button_6( unsigned char flag )
{
    if( flag == BUTTON_DOWN )
	{
		rt_kprintf("button_6 DOWN!\r\n");
		
	}
	else
		rt_kprintf("button_6 UP!\r\n");
}//---------------------------------

void button_7( unsigned char flag )
{
    if( flag == BUTTON_DOWN )
	{
		rt_kprintf("button_7 DOWN!\r\n");
	
	}
	else
	{
		rt_kprintf("button_7 UP!\r\n");

	}
}//---------------------------------
void button_8( unsigned char flag )
{
    if( flag == BUTTON_DOWN )
	{
		rt_kprintf("button_8 DOWN!\r\n");
	}
	else
		rt_kprintf("button_8 UP!\r\n");
}
//---------------------------------
void button_dummy( unsigned char flag )
{
	if( flag == BUTTON_DOWN )
	{
		rt_kprintf("button_nothing DOWN!\r\n");
	}
}


static void button_scan_entey(void *parameter)
{
	uint8_t i;
	static uint8_t new_button_status, mask_bit;
	while(1)
  {
		rt_thread_mdelay(50);
		new_button_status = get_165_data();//get_button();//
	
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
				
//		#ifdef BUTTON_LONG_PRESS
//		//long press dispose
//		else
//		{
//			for( i= BUTTON_1; i<MAX_KIN_COL; i++ )
//			{
//				if(buttons[i].status & BUTTON_PRESSED)
//				{
//					if(buttons[i].count < BUTTON_LONG_TIMER )	//40*50ms = 2000ms
//					{	
//						buttons[i].count++;
//						if(buttons[i].count == BUTTON_LONG_TIMER)
//						{	process[button_table[i]](BUTTON_LONG_PRESSED);
//							buttons[i].count--;
//						}
//					}
//				}
//				else
//				{
//					if(buttons[i].count < BUTTON_LONG_TIMER )	//10*50ms = 500ms
//					{	
//						buttons[i].count++;
//						if(buttons[i].count == BUTTON_LONG_TIMER)
//							process[button_table[i]](BUTTON_LONG_RELEASE);
//					}
//				}
//			}
//		}
//		#endif
		
		
		}
}

int thread_button_init(void)
{
	uint8_t i;
	//Button_IO_Init();
	for( i=0; i<BUTTON_NUMBER; i++ )
	{
		buttons[i].status = 0x20;
	}
	old_button_status = 0xFF & get_165_data();//get_button();////0xFF & get_165_data();
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

MSH_CMD_EXPORT(thread_button_init, thread button dev);




