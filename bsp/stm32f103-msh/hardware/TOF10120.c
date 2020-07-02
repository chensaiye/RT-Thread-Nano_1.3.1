#include "tof10120.h" 
#include "iic_bus.h" 			 
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//tof10120 驱动代码	   
////////////////////////////////////////////////////////////////////////////////// 

#define TOF_BUF_SIZE 20
#define TOF_MAX_DIS 800
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


//初始化IIC接口
int TOF10120_Init(void)
{
	uint16_t i;
	//io init
	MX_I2C1_Init();
	
	//data init
	for(i=0;i<TOF_BUF_SIZE;i++)
		TOF_Value_Buf[i]=TOF_MAX_DIS;
	TOF_Value = TOF_MAX_DIS;
	
	return 0;
}

INIT_BOARD_EXPORT(TOF10120_Init);

//在TOF10120_Write指定地址写入一个数据
//WriteAddr  :写入数据的目的地址    
//DataToWrite:要写入的数据
void TOF10120_WriteOneByte(uint16_t WriteAddr,uint8_t DataToWrite)
{				   	  	    																 
	//I2C_Master_BufferWrite(I2C1,&DataToWrite,0x01,TOF_ADDRESS1,WriteAddr);
}

//在TOF10120_Write里面的指定地址开始写入指定个数的数据
//WriteAddr :开始写入的地址 对24c02为0~255
//pBuffer   :数据数组首地址
//NumToWrite:要写入数据的个数
void TOF10120_Write(uint16_t WriteAddr,uint8_t *pBuffer,uint16_t NumToWrite)
{

}

//在TOF10120_Write里面的指定地址开始读出指定个数的数据
//ReadAddr :开始读出的地址 对24c02为0~255
//pBuffer  :数据数组首地址
//NumToRead:要读出数据的个数
void TOF10120_Read(uint16_t ReadAddr,uint8_t *pBuffer,uint16_t NumToRead)
{
	//I2C_Master_BufferRead(I2C1,pBuffer,NumToRead,TOF_ADDRESS1,ReadAddr);
	HAL_I2C_Mem_Read(&hi2c1,TOF_ADDRESS1,ReadAddr,I2C_MEMADD_SIZE_8BIT,pBuffer,NumToRead,10000);
} 

uint16_t TOF10120_Read_Distence(void)
{
	uint8_t buf[2];
//	if(I2C_Master_BufferRead(I2C1,buf,0x02,TOF_ADDRESS1,FILTERED_DIS_REG))
//		return 0;//error
//	return ((buf[0]<<8) + buf[1]);
	
	if(HAL_I2C_Mem_Read(&hi2c1,TOF_ADDRESS1,FILTERED_DIS_REG,I2C_MEMADD_SIZE_8BIT,buf,0x02,10000)==HAL_OK)
		return ((buf[0]<<8) + buf[1]);
	else
		return 0;
}
 
//去掉最大最小值，然后取平均值 
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
	TOF_Value_Buf[TOF_Buf_Set] = TOF10120_Read_Distence();
	TOF_Buf_Set++;
	if(TOF_Buf_Set > TOF_BUF_SIZE-1)
		TOF_Buf_Set=0;
	TOF_Value = Get_Buf_Average(TOF_BUF_SIZE,TOF_Value_Buf);
	return TOF_Value;
}





