#include "LM75A.h" 

//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
#define IIC_LM75A_TimeOut 10000

extern I2C_HandleTypeDef hi2c2;

extern void Error_Handler(void);

//初始化IIC接口
static int LM75A_Init(void)
{
	MX_I2C2_Init();//IIC初始化
	return 0;
}
INIT_BOARD_EXPORT(LM75A_Init);


//在AT24CXX里面的指定地址开始读出指定个数的数据
//ReadAddr :开始读出的地址 对24c02为0~255
//pBuffer  :数据数组首地址
//NumToRead:要读出数据的个数
HAL_StatusTypeDef LM75A_Read(uint16_t ReadAddr,uint8_t *pBuffer,uint16_t NumToRead)
{
	HAL_StatusTypeDef error;
	//I2C_Master_BufferRead(I2C1,pBuffer,NumToRead,0XA0,ReadAddr);
	error = HAL_I2C_Mem_Read(&hi2c2,LM75A_ADDRESS,ReadAddr,I2C_MEMADD_SIZE_8BIT,pBuffer,NumToRead,IIC_LM75A_TimeOut);
	return error;
} 


uint16_t LM75A_Read_Test(void)
{
	uint16_t temp_buf;
	//int16_t temperature;
	if(LM75A_Read(0,(uint8_t *)&temp_buf,2) == HAL_OK)
	{
		Swap16(temp_buf);	
		temp_buf = (temp_buf&0xFFE0) >> 5;//精度单位0.125°C
		if(temp_buf > 0x400)
			return 0;//负温度
	
		temp_buf = temp_buf>>3; //精度单位°C
	}
	else
		temp_buf = 0;
	return temp_buf;
}
 







