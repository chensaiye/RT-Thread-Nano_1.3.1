#include "tof10120.h" 
#include "iic_bus.h" 			 
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F407������
//tof10120 ��������	   
////////////////////////////////////////////////////////////////////////////////// 
extern void Error_Handler(void);
/**
  * @brief I2C2 Initialization Function
  * @param None
  * @retval None
  */
extern I2C_HandleTypeDef hi2c1;


//��ʼ��IIC�ӿ�
int TOF10120_Init(void)
{
	MX_I2C1_Init();
	return 0;
}

INIT_BOARD_EXPORT(TOF10120_Init);

//��TOF10120_Writeָ����ַд��һ������
//WriteAddr  :д�����ݵ�Ŀ�ĵ�ַ    
//DataToWrite:Ҫд�������
void TOF10120_WriteOneByte(uint16_t WriteAddr,uint8_t DataToWrite)
{				   	  	    																 
	//I2C_Master_BufferWrite(I2C1,&DataToWrite,0x01,TOF_ADDRESS1,WriteAddr);
}

//��TOF10120_Write�����ָ����ַ��ʼд��ָ������������
//WriteAddr :��ʼд��ĵ�ַ ��24c02Ϊ0~255
//pBuffer   :���������׵�ַ
//NumToWrite:Ҫд�����ݵĸ���
void TOF10120_Write(uint16_t WriteAddr,uint8_t *pBuffer,uint16_t NumToWrite)
{

}

//��TOF10120_Write�����ָ����ַ��ʼ����ָ������������
//ReadAddr :��ʼ�����ĵ�ַ ��24c02Ϊ0~255
//pBuffer  :���������׵�ַ
//NumToRead:Ҫ�������ݵĸ���
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
 







