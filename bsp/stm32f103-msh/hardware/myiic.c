#include "myiic.h"

/* Private function prototypes -----------------------------------------------*/
void UserI2c_DelayuS(unsigned int nCount);
/* Private function ----------------------------------------------------------*/
/*******************************************************************************
* Function Name  : UserI2c_DelayuS
* Description    : Inserts a delay time.
* Input          : nCount: specifies the delay time length.
* Output         : None
* Return         : None
*******************************************************************************/
void UserI2c_DelayuS(unsigned int nCount)
{
	//unsigned int i;
	
  for(; nCount != 0; nCount--)
	{
		//400kHZ
		__nop();
		
//		//100kHZ
//		for(i=0;i<6;i++)
//		{
//			__nop();__nop();
//		}
		
//		//30kHZ
//		for(i=0;i<25;i++)
//		{
//			__nop();__nop();
//		}
	}
}
/*******************************************************************************
* Function Name  : UserI2c_Delay30uS
* Description    : Inserts a delay 30uS.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void UserI2c_Delay30uS(void)
{
	unsigned int i;
	
  for(i=0; i<100;i++)
	{
			__nop();__nop();__nop();__nop();__nop();//
	}
}
/*******************************************************************************
* Function Name  : UserI2c_Start
* Description    : sck stable in H,sda falling edge
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void UserI2c_Start(void)
{
	USERI2C_SDA_1;	 
	UserI2c_DelayuS(5); 	  
	USERI2C_SCL_1;
	UserI2c_DelayuS(5);
 	USERI2C_SDA_0;//START:when CLK is high,DATA change form high to low 
	UserI2c_DelayuS(5);
	USERI2C_SCL_0;//钳住I2C总线，准备发送或接收数据 
	UserI2c_Delay30uS();	//加30uS延时
}	  
/*******************************************************************************
* Function Name  : UserI2c_Stop
* Description    : sck stable in H,sda rising edge
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void UserI2c_Stop(void)
{
	USERI2C_SCL_0;
	USERI2C_SDA_0;//STOP:when CLK is high DATA change form low to high
 	UserI2c_DelayuS(5);
	USERI2C_SCL_1;
 	UserI2c_DelayuS(5);
	USERI2C_SDA_1;//发送I2C总线结束信号
	UserI2c_DelayuS(5);		
	UserI2c_Delay30uS();	//加30uS延时
	
}
/*******************************************************************************
* Function Name  : UserI2c_Wait_Ack
* Description    : the 9th clock pulse period wait ack 
* Input          : None
* Output         : None
* Return         : =0有ack
*								 : =1无ack
*******************************************************************************/
unsigned char UserI2c_Wait_Ack(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	unsigned short int ucErrTime=0;
  unsigned char RetValue;
	
//	USERI2C_SDA_IN();      //SDA设置为输入   

	GPIO_InitStructure.Pin = I2C_SDA_PIN;
	GPIO_InitStructure.Mode = GPIO_MODE_INPUT ;   //上拉输入
	HAL_GPIO_Init(I2C_SDA_PORT, &GPIO_InitStructure);
	
	USERI2C_SCL_0;
	UserI2c_DelayuS(5);	
	USERI2C_SCL_1; 
  ucErrTime = 10000;
  while( ucErrTime-- > 0 )
  {
    if(USERI2C_READ_SDA )
    {
      RetValue = 0;     			
    }
    else
    {
      RetValue = 1;
			break;
    }
  }
	UserI2c_DelayuS(1);	
	USERI2C_SCL_0;//时钟输出0 	
	
//	USERI2C_SDA_OUT();//sda线输出 	
	GPIO_InitStructure.Pin = I2C_SDA_PIN;
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP ;   //推挽输出
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(I2C_SDA_PORT, &GPIO_InitStructure);
	
	USERI2C_SDA_0;  
	UserI2c_Delay30uS();	//加30uS延时
	return RetValue;  
} 
/*******************************************************************************
* Function Name  : useri2c_ack
* Description    : the 9th clock pulse period, the receiver pulls sda low to 
* 							 : acknowledge the receipt of the eight data bits.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void useri2c_ack(void)
{
	USERI2C_SCL_0;
//	USERI2C_SDA_OUT();
	USERI2C_SDA_0;
	UserI2c_DelayuS(5);
	USERI2C_SCL_1;
	UserI2c_DelayuS(5);
	USERI2C_SCL_0;
	UserI2c_Delay30uS();//加30uS延时
}

/*******************************************************************************
* Function Name  : useri2c_nack
* Description    : no acknowledge the receipt of the eight data bits.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/   
void useri2c_nack(void)
{
	USERI2C_SCL_0;
//	USERI2C_SDA_OUT();
	USERI2C_SDA_1;
	UserI2c_DelayuS(5);
	USERI2C_SCL_1;
	UserI2c_DelayuS(5);
	USERI2C_SCL_0;
	UserI2c_Delay30uS();	//加30uS延时
}					
 
/*******************************************************************************
* Function Name  : UserI2c_Send_Byte
* Description    : write one data to i2c bus
* Input          : txd-data
* Output         : None
* Return         : None
*******************************************************************************/	  
void UserI2c_Send_Byte(unsigned char txd)
{                        
    unsigned char t;   
	
    USERI2C_SCL_0;//拉低时钟开始数据传输
		UserI2c_DelayuS(5);
    for(t=0;t<8;t++)
    {      
			if((txd&0x80)>>0)
				USERI2C_SDA_1;
			else
				USERI2C_SDA_0;
			txd<<=1; 	   
			UserI2c_DelayuS(5);     
			USERI2C_SCL_1;
			UserI2c_DelayuS(5); 
			USERI2C_SCL_0;	
    }	 
} 	 
/*******************************************************************************
* Function Name  : UserI2c_Read_Byte
* Description    : read one data from i2c bus
* Input          : None
* Output         : None
* Return         : receoved data
*******************************************************************************/ 
unsigned char UserI2c_Read_Byte(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	unsigned char i,receive=0;
	
//	USERI2C_SDA_IN();//SDA设置为输入
	GPIO_InitStructure.Pin = I2C_SDA_PIN;
	GPIO_InitStructure.Mode = GPIO_MODE_INPUT ;   //上拉输入
	HAL_GPIO_Init(I2C_SDA_PORT, &GPIO_InitStructure);
	
	USERI2C_SDA_1;
	USERI2C_SCL_0;
	UserI2c_DelayuS(30);
	for(i=0;i<8;i++ )
	{ 
		receive<<=1; 
		USERI2C_SCL_1; 
		UserI2c_DelayuS(5);
		if(USERI2C_READ_SDA)
			receive++; 
		USERI2C_SCL_0;
		UserI2c_DelayuS(5); 
	}				 
	USERI2C_SCL_0;  
	
//	USERI2C_SDA_OUT();//sda线输出 
	
	GPIO_InitStructure.Pin = I2C_SDA_PIN;
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP ;   //推挽输出
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(I2C_SDA_PORT, &GPIO_InitStructure);
	return receive;
}


/*******************************************************************************
* Function Name  :SensorWritenByte
* Description    :  Sensor read api.
* Input          : devid-设备i2c地址
*                : *TXBuff-the address of the buffer which to write.
*                : SubAdd-the address read to
*                : Size-the buffer size
* Output         : None
* Return         : None
*******************************************************************************/
unsigned char SensorWritenByte(unsigned char Devid, unsigned char *TXBuff, unsigned char SubAdd, unsigned char Size)
{
	unsigned char i = 0;
  
	UserI2c_Start();

  UserI2c_Send_Byte( Devid|0x00 );
  if( 0 == UserI2c_Wait_Ack() )
  {
		UserI2c_Stop();
    return 1;
  }
  UserI2c_Send_Byte( SubAdd&0xff );
  if( 0 == UserI2c_Wait_Ack() )
  {
		UserI2c_Stop();
    return 2;
  }
	for ( i = 0; i < Size; i++)
	{
		UserI2c_Send_Byte( TXBuff[Size-i-1] );
		if( 0 == UserI2c_Wait_Ack() )
		{
			UserI2c_Stop();
			return 3;
		}
	}
  UserI2c_Stop();
	
  return 0;
}
/*******************************************************************************
* Function Name  : SensorReadnByte
* Description    :  Sensor read api.
* Input          : devid-设备i2c地址
*                : *RXBuff-the address of the buffer which stores read content.
*                : SubAdd-the address read from
*                : Size-the buffer size
* Output         : None
* Return         : None
*******************************************************************************/
unsigned char SensorReadnByte(unsigned char Devid, unsigned char *RXBuff, unsigned char SubAdd, unsigned char Size)
{
  unsigned char i = 0;
	
  UserI2c_Start();

  UserI2c_Send_Byte( Devid|0x00 );
  if( 0 == UserI2c_Wait_Ack() )
  {
  UserI2c_Stop();
    return 1;
  }
  UserI2c_Send_Byte( SubAdd&0xff );
  if( 0 == UserI2c_Wait_Ack() )
  {
  UserI2c_Stop();
    return 2;
  }
	UserI2c_Stop();
  
  UserI2c_Start();
  UserI2c_Send_Byte( Devid|0x01 );
  if( 0 == UserI2c_Wait_Ack() )
  {
//  UserI2c_Stop();
//    return 0;
  }
	for ( i = 0; i < Size; i++)
	{
		RXBuff[Size-i-1] = UserI2c_Read_Byte();	
		if((i+1)==Size)			
			useri2c_nack();
		else
			useri2c_ack();
	}
  UserI2c_Stop();
	
  return 0;
}
/*******************************************************************************
* Function Name  : UserI2c_Init
* Description    : config i2c driver gpio
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void UserI2c_Init(void)
{					     
	GPIO_InitTypeDef GPIO_InitStructure;
	__HAL_RCC_GPIOB_CLK_ENABLE();
		   
	GPIO_InitStructure.Pin = I2C_SCL_PIN;
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP ;   //推挽输出
	//GPIO_InitStructure.Pull  = GPIO_NOPULL;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(I2C_SCL_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.Pin = I2C_SDA_PIN;
	HAL_GPIO_Init(I2C_SDA_PORT, &GPIO_InitStructure);
	
	HAL_GPIO_WritePin(I2C_SCL_PORT, I2C_SCL_PIN, GPIO_PIN_SET);//SCL 输出高
	HAL_GPIO_WritePin(I2C_SDA_PORT, I2C_SDA_PIN, GPIO_PIN_SET);//SDA 输出高
		 
}

