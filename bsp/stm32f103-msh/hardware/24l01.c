#include "24l01.h"
#include "spi_bus.h"
#include <rthw.h>
#include <rtthread.h>

#include "db.h"

uint8_t TX_ADDRESS[TX_ADR_WIDTH]= {0x80,'-','Y','C','C'}; //发送地址
uint8_t RX0_ADDRESS[RX_ADR_WIDTH]={0x80,'-','Y','C','C'}; //接受地址
uint8_t RX1_ADDRESS[RX_ADR_WIDTH]={0x10,'-','Y','C','C'}; //接受地址  "YCC-"
uint8_t NRF_RX_BUF[RX_PLOAD_WIDTH+1];

uint8_t MYADDR;
uint8_t NRF_ERROR=0;

extern SPI_HandleTypeDef hspi1;
extern void rt_hw_us_delay(rt_uint32_t us);
//初始化24L01的IO口

void NRF24L01_Init(void)
{  
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	/* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
	
	/*Configure GPIO pins :  CE_NRF_Pin CSN_NRF_Pin IRQ_NRF_Pin */
  GPIO_InitStruct.Pin = CE_NRF_Pin|CSN_NRF_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin = IRQ_NRF_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(IRQ_NRF_GPIO_Port, &GPIO_InitStruct);
	
	/*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, CE_NRF_Pin, GPIO_PIN_RESET);//使能24L01
	HAL_GPIO_WritePin(GPIOC, CSN_NRF_Pin, GPIO_PIN_SET);//SPI片选取消
	
	MX_SPI1_Init();
	HAL_SPI_Transmit(&hspi1,&MYADDR,1,10000);//未知原因，SPI初始化完成后必须启动发送一次，后续Check才能通过
	
	if( NRF24L01_Check())
		NRF_ERROR = 1; //HAL_UART_Transmit(&huart2,"nrf240l01 error\r\n",17,10000);
	else
	{
		NRF24L01_ModeInt();
		NRF24L01_RX_Mode();
	}
	
}
//检测24L01是否存在
//返回值:0，成功;1，失败	
uint8_t NRF24L01_Check(void)
{
	uint8_t buf[5]={0XA5,0XA5,0XA5,0XA5,0XA5};
	uint8_t i;
	NRF24L01_Write_Buf(NRF_WRITE_REG+TX_ADDR,buf,5);//写入5个字节的地址.	
	NRF24L01_Read_Buf(TX_ADDR,buf,5); //读出写入的地址  
	for(i=0;i<5;i++)if(buf[i]!=0XA5)break;	 							   
	if(i!=5)return 1;//检测24L01错误	
	return 0;		 //检测到24L01
}	

//SPI写寄存器
//reg:指定寄存器地址
//value:写入的值
uint8_t NRF24L01_Write_Reg(uint8_t reg,uint8_t value)
{
	uint8_t status;	
	HAL_GPIO_WritePin(GPIOC, CSN_NRF_Pin, GPIO_PIN_RESET);//使能24L01
	rt_hw_us_delay(8);
	status = HAL_SPI_Transmit(&hspi1,(uint8_t *)&reg,1,10000);//发送寄存器号 
	status = HAL_SPI_Transmit(&hspi1,(uint8_t *)&value,1,10000);//写入寄存器的值 
	HAL_GPIO_WritePin(GPIOC, CSN_NRF_Pin, GPIO_PIN_SET);//SPI片选取消
	rt_hw_us_delay(8);
	return(status);       			//返回状态值
}
//读取SPI寄存器值
//reg:要读的寄存器
uint8_t NRF24L01_Read_Reg(uint8_t reg)
{
	uint8_t status,reg_val;	    
 	HAL_GPIO_WritePin(GPIOC, CSN_NRF_Pin, GPIO_PIN_RESET);//使能24L01
 	rt_hw_us_delay(8);
	status = HAL_SPI_Transmit(&hspi1,(uint8_t *)&reg,1,10000);//发送寄存器号 
	status = HAL_SPI_Receive(&hspi1,(uint8_t *)&reg_val,1,10000);//读取寄存器内容
	HAL_GPIO_WritePin(GPIOC, CSN_NRF_Pin, GPIO_PIN_SET);//SPI片选取消	
  rt_hw_us_delay(8);
	if(status)
		return status;		//返回错误值
  else
		return(reg_val);           //返回读取值
}	
//在指定位置读出指定长度的数据
//reg:寄存器(位置)
//*pBuf:数据指针
//len:数据长度
//返回值,此次读到的状态寄存器值 
uint8_t NRF24L01_Read_Buf(uint8_t reg,uint8_t *pBuf,uint8_t len)
{
	uint8_t status;	       
	HAL_GPIO_WritePin(GPIOC, CSN_NRF_Pin, GPIO_PIN_RESET);//使能24L01
	rt_hw_us_delay(8);
	status = HAL_SPI_Transmit(&hspi1,(uint8_t *)&reg,1,10000);//发送寄存器号 
	status = HAL_SPI_Receive(&hspi1,pBuf,len,10000);//读取寄存器内容
	HAL_GPIO_WritePin(GPIOC, CSN_NRF_Pin, GPIO_PIN_SET);//SPI片选取消	delay_us(8);
	return status;        //返回读到的状态值
}
//在指定位置写指定长度的数据
//reg:寄存器(位置)
//*pBuf:数据指针
//len:数据长度
//返回值,此次读到的状态寄存器值
uint8_t NRF24L01_Write_Buf(uint8_t reg, uint8_t *pBuf, uint8_t len)
{
	uint8_t status;	    
 	HAL_GPIO_WritePin(GPIOC, CSN_NRF_Pin, GPIO_PIN_RESET);//使能24L01
 	rt_hw_us_delay(8);
  status = HAL_SPI_Transmit(&hspi1,(uint8_t *)&reg,1,10000);//发送寄存器号 
	status = HAL_SPI_Transmit(&hspi1,pBuf,len,10000);//发送寄存器号 
	HAL_GPIO_WritePin(GPIOC, CSN_NRF_Pin, GPIO_PIN_SET);//SPI片选取消	delay_us(8);delay_us(8);
  return status;          //返回读到的状态值
}				   
//启动NRF24L01发送一次数据
//txbuf:待发送数据首地址
//返回值:发送完成状况
uint8_t NRF24L01_TxPacket(uint8_t *txbuf)
{
	uint8_t sta;
 	HAL_GPIO_WritePin(GPIOC, CE_NRF_Pin, GPIO_PIN_RESET);
	rt_hw_us_delay(8);
  NRF24L01_Write_Buf(WR_TX_PLOAD,txbuf,TX_PLOAD_WIDTH);//写数据到TX BUF  32个字节
 	HAL_GPIO_WritePin(GPIOC, CE_NRF_Pin, GPIO_PIN_SET);//启动发送	   
 	rt_hw_us_delay(8);
	while(HAL_GPIO_ReadPin(IRQ_NRF_GPIO_Port,IRQ_NRF_Pin)!=0);//等待发送完成
	sta=NRF24L01_Read_Reg(STATUS);  //读取状态寄存器的值	   
	NRF24L01_Write_Reg(NRF_WRITE_REG+STATUS,sta); //清除TX_DS或MAX_RT中断标志
	if(sta&MAX_TX)//达到最大重发次数
	{
		NRF24L01_Write_Reg(FLUSH_TX,0xff);//清除TX FIFO寄存器 
		//sta=NRF24L01_Read_Reg(STATUS);  //读取状态寄存器的值	 
		return MAX_TX; 
	}
	if(sta&TX_OK)//发送完成
	{
		return TX_OK;
	}
	return 0xff;//其他原因发送失败
}
//启动NRF24L01发送一次数据
//txbuf:待发送数据首地址
//返回值:0，接收完成；其他，错误代码
uint8_t NRF24L01_RxPacket(uint8_t *rxbuf)
{
	uint8_t sta;		    							   
	sta=NRF24L01_Read_Reg(STATUS);  //读取状态寄存器的值    	 
	if(sta&RX_OK)//接收到数据
	{
		NRF24L01_Read_Buf(RD_RX_PLOAD,rxbuf,RX_PLOAD_WIDTH);//读取数据
		NRF24L01_Write_Reg(FLUSH_RX,0xff);//清除RX FIFO寄存器 
		NRF24L01_Write_Reg(NRF_WRITE_REG+STATUS,sta); //清除TX_DS或MAX_RT中断标志
		return 0; 
	}	
	return 1;//没收到任何数据
}


void NRF24L01_ModeInt(void)
{	
	MYADDR = 0;
	HAL_GPIO_WritePin(GPIOC, CE_NRF_Pin, GPIO_PIN_RESET);//  
	rt_hw_us_delay(8);
  	NRF24L01_Write_Buf(NRF_WRITE_REG+RX_ADDR_P0,(uint8_t*)RX0_ADDRESS,RX_ADR_WIDTH);//写RX节点地址
		NRF24L01_Write_Buf(NRF_WRITE_REG+RX_ADDR_P1,(uint8_t*)RX1_ADDRESS,RX_ADR_WIDTH);//写RX1节点地址

		NRF24L01_Write_Reg(NRF_WRITE_REG+RX_ADDR_P2,0x20);
		NRF24L01_Write_Reg(NRF_WRITE_REG+RX_ADDR_P3,0x30);
		NRF24L01_Write_Reg(NRF_WRITE_REG+RX_ADDR_P4,0x40);
		NRF24L01_Write_Reg(NRF_WRITE_REG+RX_ADDR_P5,0x50);
	
  	NRF24L01_Write_Reg(NRF_WRITE_REG+EN_AA,0x3F);    	//使能通道0-5的自动应答  ,禁止自动应答  
  	NRF24L01_Write_Reg(NRF_WRITE_REG+EN_RXADDR,0x3F);	//使能通道0-5的接收允许  	 
  	NRF24L01_Write_Reg(NRF_WRITE_REG+RF_CH,((MYADDR &0x0f) + 0x20));	    	//设置RF通信频率		
	
		NRF24L01_Write_Reg(NRF_WRITE_REG+SETUP_RETR,0x07);//设置自动重发间隔时间:250us + 86us;最大自动重发次数:16（f）次	
  	NRF24L01_Write_Reg(NRF_WRITE_REG+RX_PW_P0,RX_PLOAD_WIDTH);//选择通道0的有效数据宽度 	
		NRF24L01_Write_Reg(NRF_WRITE_REG+RX_PW_P1,RX_PLOAD_WIDTH);//选择通道1的有效数据宽度 
		NRF24L01_Write_Reg(NRF_WRITE_REG+RX_PW_P2,RX_PLOAD_WIDTH);//选择通道2的有效数据宽度 
		NRF24L01_Write_Reg(NRF_WRITE_REG+RX_PW_P3,RX_PLOAD_WIDTH);//选择通道3的有效数据宽度 
		NRF24L01_Write_Reg(NRF_WRITE_REG+RX_PW_P4,RX_PLOAD_WIDTH);//选择通道4的有效数据宽度 
		NRF24L01_Write_Reg(NRF_WRITE_REG+RX_PW_P5,RX_PLOAD_WIDTH);//选择通道5的有效数据宽度 
	
  	NRF24L01_Write_Reg(NRF_WRITE_REG+RF_SETUP,0x0f);	//设置TX发射参数,0db增益,2Mbps,低噪声增益开启   
	
		NRF24L01_Write_Reg(NRF_WRITE_REG+CONFIG, 0x0f);		//配置基本工作模式的参数;PWR_UP,EN_CRC,16BIT_CRC,接收模式 
  	HAL_GPIO_WritePin(GPIOC, CE_NRF_Pin, GPIO_PIN_SET);//	 //CE为高,进入接收模式 
  rt_hw_us_delay(8);
}

//该函数初始化NRF24L01到RX模式
//设置RX地址,写RX数据宽度,选择RF频道,波特率和LNA HCURR
//当CE变高后,即进入RX模式,并可以接收数据了		   
void NRF24L01_RX_Mode(void)
{
	HAL_GPIO_WritePin(GPIOC, CE_NRF_Pin, GPIO_PIN_RESET);
	rt_hw_us_delay(8);
  	NRF24L01_Write_Buf(NRF_WRITE_REG+RX_ADDR_P0,(uint8_t*)RX0_ADDRESS,RX_ADR_WIDTH);//写RX节点地址
	  NRF24L01_Write_Reg(NRF_WRITE_REG+CONFIG, 0x0f);		//配置基本工作模式的参数;PWR_UP,EN_CRC,16BIT_CRC,接收模式 
	
  HAL_GPIO_WritePin(GPIOC, CE_NRF_Pin, GPIO_PIN_SET);//使能24L01  
  rt_hw_us_delay(8);
}						 
//该函数初始化NRF24L01到TX模式
//设置TX地址,写TX数据宽度,设置RX自动应答的地址,填充TX发送数据,选择RF频道,波特率和LNA HCURR
//PWR_UP,CRC使能
//当CE变高后,即进入RX模式,并可以接收数据了		   
//CE为高大于10us,则启动发送.	 
void NRF24L01_TX_Mode(void)
{														 
	HAL_GPIO_WritePin(GPIOC, CE_NRF_Pin, GPIO_PIN_RESET);
	rt_hw_us_delay(8);	
	NRF24L01_Write_Buf(NRF_WRITE_REG+TX_ADDR,(uint8_t*)TX_ADDRESS,TX_ADR_WIDTH);//写TX节点地址 
  	NRF24L01_Write_Buf(NRF_WRITE_REG+RX_ADDR_P0,(uint8_t*)TX_ADDRESS,TX_ADR_WIDTH); //设置TX节点地址,主要为了使能ACK	  
	NRF24L01_Write_Reg(NRF_WRITE_REG+CONFIG,0x0e);    //配置基本工作模式的参数;PWR_UP,EN_CRC,16BIT_CRC,接收模式,开启所有中断
	HAL_GPIO_WritePin(GPIOC, CE_NRF_Pin, GPIO_PIN_SET);//使能24L01  //CE为高,10us后启动发送
	rt_hw_us_delay(8);
	
}

#define NRF_THREAD_PRIORITY         (RT_THREAD_PRIORITY_MAX - 1)
#define NRF_THREAD_STACK_SIZE       512
#define NRF_THREAD_TIMESLICE        20

ALIGN(RT_ALIGN_SIZE)
static char thd_nrf_stack[NRF_THREAD_STACK_SIZE];
static struct rt_thread thd_nrf;

static void NRF_scan_entey(void *parameter)
{
	while(1)
  {
		rt_thread_mdelay(100);
		//通过nrf24l01调试
		if(NRF_ERROR==0)
		{
			if(NRF24L01_RxPacket(NRF_RX_BUF)==0)
			{
				NRF_RX_BUF[RX_PLOAD_WIDTH]=0;
				db_dev.scan(NRF_RX_BUF);
			}
		}
	}
}
int thread_nrf_init(void)
{
	
	NRF24L01_Init();
	
	if(NRF_ERROR==0)
	{	
			rt_kprintf("NRF24L01 INIT OK!\r\n");
		 rt_thread_init(&thd_nrf,
										 "thd_nrf",
										 NRF_scan_entey,
										 RT_NULL,
										 &thd_nrf_stack[0],
										 sizeof(thd_nrf_stack),
										 NRF_THREAD_PRIORITY - 1, NRF_THREAD_TIMESLICE);
										 
		 rt_thread_startup(&thd_nrf);
	}
    return 0;
}

//MSH_CMD_EXPORT(thread_nrf_init, thread nrf debug);

