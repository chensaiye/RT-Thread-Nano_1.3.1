#include "LM75A.h" 

//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
#define IIC_LM75A_TimeOut 10000

extern I2C_HandleTypeDef hi2c2;

extern void Error_Handler(void);

//��ʼ��IIC�ӿ�
static int LM75A_Init(void)
{
	MX_I2C2_Init();//IIC��ʼ��
	return 0;
}
INIT_BOARD_EXPORT(LM75A_Init);


//��AT24CXX�����ָ����ַ��ʼ����ָ������������
//ReadAddr :��ʼ�����ĵ�ַ ��24c02Ϊ0~255
//pBuffer  :���������׵�ַ
//NumToRead:Ҫ�������ݵĸ���
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
		temp_buf = (temp_buf&0xFFE0) >> 5;//���ȵ�λ0.125��C
		if(temp_buf > 0x400)
			return 0;//���¶�
	
		temp_buf = temp_buf>>3; //���ȵ�λ��C
	}
	else
		temp_buf = 0;
	return temp_buf;
}
 







