#include "24cxx.h" 

//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//24CXX 驱动代码	   
////////////////////////////////////////////////////////////////////////////////// 	
#define PageSize 0x08		//页大小
#define Slave_Address 0xA0		//页大小
#define IIC_24Cxx_TimeOut 10000

extern I2C_HandleTypeDef hi2c2;

extern void Error_Handler(void);

//初始化IIC接口
static int AT24CXX_Init(void)
{
	MX_I2C2_Init();//IIC初始化
	return 0;
}
//INIT_BOARD_EXPORT(AT24CXX_Init);

//在AT24CXX指定地址写入一个数据
//WriteAddr  :写入数据的目的地址    
//DataToWrite:要写入的数据
void AT24CXX_WriteOneByte(uint16_t WriteAddr,uint8_t DataToWrite)
{				   	  	    																 
	//I2C_Master_BufferWrite(I2C1,&DataToWrite,0x01,0XA0,WriteAddr);
	HAL_I2C_Mem_Write(&hi2c2,Slave_Address,WriteAddr,I2C_MEMADD_SIZE_8BIT,&DataToWrite,1,IIC_24Cxx_TimeOut);
}

//在AT24CXX里面的指定地址开始写入一页数据
//WriteAddr  :写入数据的目的地址    
//pBuffer		 :要写入的数据
void AT24CXX_WriteOnePage(uint16_t WriteAddr,uint8_t *pBuffer)//uint16_t NumToWrite)
{
	//I2C_Master_BufferWrite(I2C1,pBuffer,PageSize,0XA0,WriteAddr);
	HAL_I2C_Mem_Write(&hi2c2,Slave_Address,WriteAddr,I2C_MEMADD_SIZE_8BIT,pBuffer,PageSize,IIC_24Cxx_TimeOut);
}

//在AT24CXX里面的指定地址开始写入指定个数的数据
//WriteAddr :开始写入的地址 对24c02为0~255
//pBuffer   :数据数组首地址
//NumToWrite:要写入数据的个数
void AT24CXX_Write(uint16_t WriteAddr,uint8_t *pBuffer,uint16_t NumToWrite)
{
	uint8_t pagestart,writesize;
//	 while(NumToWrite--)
//   {
//			AT24CXX_WriteOneByte(WriteAddr,*pBuffer);
//			WriteAddr++;
//			pBuffer++;
//			delay_ms(5);//可以防止写入时死机
//	}
	while(NumToWrite)
	{
		pagestart = WriteAddr%PageSize;			//计算页起始地址
		writesize = PageSize - pagestart;		//计算页剩余字节数
		if(NumToWrite<=writesize)				//比较总剩余直接数
			writesize = NumToWrite;				//计算总剩余
		//I2C_Master_BufferWrite(I2C1,pBuffer,writesize,0XA0,WriteAddr);	//写入
		HAL_I2C_Mem_Write(&hi2c2,Slave_Address,WriteAddr,I2C_MEMADD_SIZE_8BIT,pBuffer,writesize,IIC_24Cxx_TimeOut);
		
		NumToWrite -= writesize;				//重新计算总剩余字节数
		WriteAddr += writesize;					//重新计算起始地址
		pBuffer += writesize;						//偏移数据地址
		
//		if(NumToWrite)
//			delay_ms(3);//可以防止写入时死机
	}
}

//在AT24CXX里面的指定地址开始读出指定个数的数据
//ReadAddr :开始读出的地址 对24c02为0~255
//pBuffer  :数据数组首地址
//NumToRead:要读出数据的个数
void AT24CXX_Read(uint16_t ReadAddr,uint8_t *pBuffer,uint16_t NumToRead)
{
	//I2C_Master_BufferRead(I2C1,pBuffer,NumToRead,0XA0,ReadAddr);
	HAL_I2C_Mem_Read(&hi2c2,Slave_Address,ReadAddr,I2C_MEMADD_SIZE_8BIT,pBuffer,NumToRead,IIC_24Cxx_TimeOut);
} 
//检查AT24CXX是否正常
//这里用了24XX的最后一个地址(255)来存储标志字.
//如果用其他24C系列,这个地址要修改
//返回1:检测失败
//返回0:检测成功
uint8_t AT24CXX_Check(void)
{
	uint8_t temp;
	AT24CXX_Read(255,&temp,1);//避免每次开机都写AT24CXX			   
	if(temp==0X55)return 0;		   
	else//排除第一次初始化的情况
	{
		AT24CXX_WriteOneByte(255,0X55);
	  AT24CXX_Read(255,&temp,1);	  
		if(temp==0X55)return 0;
	}
	return 1;											  
}

 







