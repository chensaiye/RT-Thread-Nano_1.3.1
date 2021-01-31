#include "tof10120.h" 
#include "iic_bus.h" 
#include "button.h"
#include "main_control.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//tof10120 驱动代码	   
////////////////////////////////////////////////////////////////////////////////// 

#define TOF_BUF_SIZE 20
#define TOF_MAX_DIS 800
uint16_t TOF_Error=0;
uint16_t TOF_Buf_Set=0;
uint16_t TOF_Value_Buf[TOF_BUF_SIZE];
uint16_t TOF_Value;

extern void Error_Handler(void);
/**
  * @brief I2C2 Initialization Function
  * @param None
  * @retval None
  */
extern I2C_HandleTypeDef hi2c1;
extern uint8_t Channels_RIR[CHANNEL_RIR_MAX];
extern union_status curr_status;//当前状态

void TOF10120_EN_Init(void)
{
   GPIO_InitTypeDef  GPIO_InitStructure;
	//EN端口设置
	 __HAL_RCC_GPIOA_CLK_ENABLE();
	  GPIO_InitStructure.Pin   = TOF1010_EN;
    GPIO_InitStructure.Mode  = GPIO_MODE_OUTPUT_PP;//推挽输出
    GPIO_InitStructure.Pull  = GPIO_NOPULL;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_LOW;
	 HAL_GPIO_Init(TOF1010_EN_Port, &GPIO_InitStructure);					 //根据设定参数初始化GPIOC
	 HAL_GPIO_WritePin(TOF1010_EN_Port,TOF1010_EN,GPIO_PIN_SET);
}
//初始化IIC接口
int TOF10120_Init(void)
{
	uint8_t i;
	TOF10120_EN_Init();
	rt_thread_mdelay(50);
	HAL_GPIO_WritePin(TOF1010_EN_Port,TOF1010_EN,GPIO_PIN_RESET);
	//io init
	MX_I2C1_Init();
	
	//data init
	for(i=0;i<TOF_BUF_SIZE;i++)
		TOF_Value_Buf[i]=TOF_MAX_DIS;
	TOF_Value = TOF_MAX_DIS;
	rt_thread_mdelay(500);
	//i=TOF_ADDRESS1;
	if(TOF10120_Read(TOF_ADDRESS_REG,&i,1)!= HAL_OK)
	//if(TOF10120_Read_Distence() == 0)
	{
		TOF_Error = 1;
		rt_kprintf("TOF Error!\r\n");
	}
	return 0;
}

//INIT_BOARD_EXPORT(TOF10120_Init);

//在TOF10120_Write指定地址写入一个数据
//WriteAddr  :写入数据的目的地址    
//DataToWrite:要写入的数据
void TOF10120_WriteOneByte(uint16_t WriteAddr,uint8_t DataToWrite)
{				   	  	    																 
	//I2C_Master_BufferWrite(I2C1,&DataToWrite,0x01,TOF_ADDRESS1,WriteAddr);
	//HAL_I2C_Master_Transmit
}

//在TOF10120_Write里面的指定地址开始写入指定个数的数据
//WriteAddr :开始写入的地址 对24c02为0~255
//pBuffer   :数据数组首地址
//NumToWrite:要写入数据的个数
HAL_StatusTypeDef TOF10120_Write(uint16_t WriteAddr,uint8_t *pBuffer,uint16_t NumToWrite)
{
	HAL_StatusTypeDef tperror = HAL_OK;
	tperror =	HAL_I2C_Mem_Write(&hi2c1,TOF_ADDRESS1,WriteAddr,I2C_MEMADD_SIZE_8BIT,pBuffer,NumToWrite,100);
	return tperror;
}

//在TOF10120_Write里面的指定地址开始读出指定个数的数据
//ReadAddr :开始读出的地址 对24c02为0~255
//pBuffer  :数据数组首地址
//NumToRead:要读出数据的个数
HAL_StatusTypeDef TOF10120_Read(uint16_t ReadAddr,uint8_t *pBuffer,uint16_t NumToRead)
{
	HAL_StatusTypeDef tperror = HAL_OK;
	//I2C_Master_BufferRead(I2C1,pBuffer,NumToRead,TOF_ADDRESS1,ReadAddr);
	tperror = HAL_I2C_Mem_Read(&hi2c1,TOF_ADDRESS1,ReadAddr,I2C_MEMADD_SIZE_8BIT,pBuffer,NumToRead,100);
	return tperror;
} 

uint32_t HAL_GetTick(void)
{
  return rt_tick_get();
}

uint16_t TOF10120_Read_Distence(void)
{
	uint8_t buf[2];
//	if(I2C_Master_BufferRead(I2C1,buf,0x02,TOF_ADDRESS1,FILTERED_DIS_REG))
//		return 0;//error
//	return ((buf[0]<<8) + buf[1]);
	
	if(HAL_I2C_Mem_Read(&hi2c1,TOF_ADDRESS1,FILTERED_DIS_REG,I2C_MEMADD_SIZE_8BIT,buf,0x02,10)==HAL_OK)
		return ((buf[0]<<8) + buf[1]);
	else
		return 1000;
}
 
//平均值滤波
//len > 2 
uint16_t Get_Buf_Average(uint16_t len,uint16_t* buf)
{
	uint16_t min=5000,max=0,i;
	uint32_t temp32=0;
	uint16_t temp16=0;
	for(i=0;i<len;i++,buf++)
	{
		if(*buf < min)
			min = *buf;
		if(*buf > max)
			max = *buf;
		temp32 += *buf;
	}
	temp32 -= (min+max);
	temp32 = temp32/(len-2);
	temp16 = (uint16_t)temp32;
	return temp16;
}

uint16_t TOF10120_Read_Scan(void)
{	
	if(TOF_Error)
		TOF_Value_Buf[TOF_Buf_Set] = 1000;
	else
		TOF_Value_Buf[TOF_Buf_Set] = TOF10120_Read_Distence();
	TOF_Buf_Set++;
	if(TOF_Buf_Set > TOF_BUF_SIZE-1)
		TOF_Buf_Set=0;
	TOF_Value = Get_Buf_Average(TOF_BUF_SIZE,TOF_Value_Buf);
	return TOF_Value;
}


#define TOFREAD_THREAD_PRIORITY         (RT_THREAD_PRIORITY_MAX - 2)
#define TOFREAD_THREAD_STACK_SIZE       256
#define TOFREAD_THREAD_TIMESLICE        20

ALIGN(RT_ALIGN_SIZE)
static char thd_tofread_stack[TOFREAD_THREAD_STACK_SIZE];
static struct rt_thread thd_tofread;


static void Tofread_scan_entey(void *parameter)
{
	uint16_t distance;
	while(1)
  {
		TOF10120_Init();
		if(TOF_Error==1)
		{
			rt_thread_mdelay(200);
		}
		else
		{
			while(1)
			{
				distance = TOF10120_Read_Scan();
				if(distance == 1000)
					break;
				if((distance>425) && Channels_RIR[6])
					bt_rir_7(BUTTON_LONG_RELEASE);
				if((distance<=425) && Channels_RIR[6]==0)
					bt_rir_7(BUTTON_LONG_PRESSED);
				rt_thread_mdelay(200);
			}
		}
	}
}

int tof_read_start(void)
{
	if((curr_status.value.sys_set & 0x08)==0x00)
		return 1;
//	 TOF10120_Init();
//	if(TOF_Error==1)
//		return 1;
	 rt_thread_init(&thd_tofread,
									 "tof_read",
									 Tofread_scan_entey,
									 RT_NULL,
									 &thd_tofread_stack[0],
									 sizeof(thd_tofread_stack),
									 TOFREAD_THREAD_PRIORITY-1, TOFREAD_THREAD_TIMESLICE);
									 
	 rt_thread_startup(&thd_tofread);
	
    return 0;
}

MSH_CMD_EXPORT(tof_read_start, thread tof read);

int tof_read_stop(void)
{
	rt_err_t error;
	error = rt_thread_detach(&thd_tofread);
	if(error ==RT_EOK)
		rt_kprintf("delete OK!\r\n");
	return 0;
}

MSH_CMD_EXPORT(tof_read_stop, thread tof read stop);


