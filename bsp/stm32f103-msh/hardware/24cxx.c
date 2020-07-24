#include "24cxx.h" 

//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F407������
//24CXX ��������	   
////////////////////////////////////////////////////////////////////////////////// 	
#define PageSize 0x08		//ҳ��С
#define Slave_Address 0xA0		//ҳ��С
#define IIC_24Cxx_TimeOut 10000

extern I2C_HandleTypeDef hi2c2;

extern void Error_Handler(void);

//��ʼ��IIC�ӿ�
static int AT24CXX_Init(void)
{
	MX_I2C2_Init();//IIC��ʼ��
	return 0;
}
//INIT_BOARD_EXPORT(AT24CXX_Init);

//��AT24CXXָ����ַд��һ������
//WriteAddr  :д�����ݵ�Ŀ�ĵ�ַ    
//DataToWrite:Ҫд�������
void AT24CXX_WriteOneByte(uint16_t WriteAddr,uint8_t DataToWrite)
{				   	  	    																 
	//I2C_Master_BufferWrite(I2C1,&DataToWrite,0x01,0XA0,WriteAddr);
	HAL_I2C_Mem_Write(&hi2c2,Slave_Address,WriteAddr,I2C_MEMADD_SIZE_8BIT,&DataToWrite,1,IIC_24Cxx_TimeOut);
}

//��AT24CXX�����ָ����ַ��ʼд��һҳ����
//WriteAddr  :д�����ݵ�Ŀ�ĵ�ַ    
//pBuffer		 :Ҫд�������
void AT24CXX_WriteOnePage(uint16_t WriteAddr,uint8_t *pBuffer)//uint16_t NumToWrite)
{
	//I2C_Master_BufferWrite(I2C1,pBuffer,PageSize,0XA0,WriteAddr);
	HAL_I2C_Mem_Write(&hi2c2,Slave_Address,WriteAddr,I2C_MEMADD_SIZE_8BIT,pBuffer,PageSize,IIC_24Cxx_TimeOut);
}

//��AT24CXX�����ָ����ַ��ʼд��ָ������������
//WriteAddr :��ʼд��ĵ�ַ ��24c02Ϊ0~255
//pBuffer   :���������׵�ַ
//NumToWrite:Ҫд�����ݵĸ���
void AT24CXX_Write(uint16_t WriteAddr,uint8_t *pBuffer,uint16_t NumToWrite)
{
	uint8_t pagestart,writesize;
//	 while(NumToWrite--)
//   {
//			AT24CXX_WriteOneByte(WriteAddr,*pBuffer);
//			WriteAddr++;
//			pBuffer++;
//			delay_ms(5);//���Է�ֹд��ʱ����
//	}
	while(NumToWrite)
	{
		pagestart = WriteAddr%PageSize;			//����ҳ��ʼ��ַ
		writesize = PageSize - pagestart;		//����ҳʣ���ֽ���
		if(NumToWrite<=writesize)				//�Ƚ���ʣ��ֱ����
			writesize = NumToWrite;				//������ʣ��
		//I2C_Master_BufferWrite(I2C1,pBuffer,writesize,0XA0,WriteAddr);	//д��
		HAL_I2C_Mem_Write(&hi2c2,Slave_Address,WriteAddr,I2C_MEMADD_SIZE_8BIT,pBuffer,writesize,IIC_24Cxx_TimeOut);
		
		NumToWrite -= writesize;				//���¼�����ʣ���ֽ���
		WriteAddr += writesize;					//���¼�����ʼ��ַ
		pBuffer += writesize;						//ƫ�����ݵ�ַ
		
//		if(NumToWrite)
//			delay_ms(3);//���Է�ֹд��ʱ����
	}
}

//��AT24CXX�����ָ����ַ��ʼ����ָ������������
//ReadAddr :��ʼ�����ĵ�ַ ��24c02Ϊ0~255
//pBuffer  :���������׵�ַ
//NumToRead:Ҫ�������ݵĸ���
void AT24CXX_Read(uint16_t ReadAddr,uint8_t *pBuffer,uint16_t NumToRead)
{
	//I2C_Master_BufferRead(I2C1,pBuffer,NumToRead,0XA0,ReadAddr);
	HAL_I2C_Mem_Read(&hi2c2,Slave_Address,ReadAddr,I2C_MEMADD_SIZE_8BIT,pBuffer,NumToRead,IIC_24Cxx_TimeOut);
} 
//���AT24CXX�Ƿ�����
//��������24XX�����һ����ַ(255)���洢��־��.
//���������24Cϵ��,�����ַҪ�޸�
//����1:���ʧ��
//����0:���ɹ�
uint8_t AT24CXX_Check(void)
{
	uint8_t temp;
	AT24CXX_Read(255,&temp,1);//����ÿ�ο�����дAT24CXX			   
	if(temp==0X55)return 0;		   
	else//�ų���һ�γ�ʼ�������
	{
		AT24CXX_WriteOneByte(255,0X55);
	  AT24CXX_Read(255,&temp,1);	  
		if(temp==0X55)return 0;
	}
	return 1;											  
}

 







