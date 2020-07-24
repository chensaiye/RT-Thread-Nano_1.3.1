#include "24l01.h"
#include "spi_bus.h"
#include <rthw.h>
#include <rtthread.h>

#include "db.h"

uint8_t TX_ADDRESS[TX_ADR_WIDTH]= {0x80,'-','Y','C','C'}; //���͵�ַ
uint8_t RX0_ADDRESS[RX_ADR_WIDTH]={0x80,'-','Y','C','C'}; //���ܵ�ַ
uint8_t RX1_ADDRESS[RX_ADR_WIDTH]={0x10,'-','Y','C','C'}; //���ܵ�ַ  "YCC-"
uint8_t NRF_RX_BUF[RX_PLOAD_WIDTH+1];

uint8_t MYADDR;
uint8_t NRF_ERROR=0;

extern SPI_HandleTypeDef hspi1;
extern void rt_hw_us_delay(rt_uint32_t us);
//��ʼ��24L01��IO��

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
  HAL_GPIO_WritePin(GPIOC, CE_NRF_Pin, GPIO_PIN_RESET);//ʹ��24L01
	HAL_GPIO_WritePin(GPIOC, CSN_NRF_Pin, GPIO_PIN_SET);//SPIƬѡȡ��
	
	MX_SPI1_Init();
	HAL_SPI_Transmit(&hspi1,&MYADDR,1,10000);//δ֪ԭ��SPI��ʼ����ɺ������������һ�Σ�����Check����ͨ��
	
	if( NRF24L01_Check())
		NRF_ERROR = 1; //HAL_UART_Transmit(&huart2,"nrf240l01 error\r\n",17,10000);
	else
	{
		NRF24L01_ModeInt();
		NRF24L01_RX_Mode();
	}
	
}
//���24L01�Ƿ����
//����ֵ:0���ɹ�;1��ʧ��	
uint8_t NRF24L01_Check(void)
{
	uint8_t buf[5]={0XA5,0XA5,0XA5,0XA5,0XA5};
	uint8_t i;
	NRF24L01_Write_Buf(NRF_WRITE_REG+TX_ADDR,buf,5);//д��5���ֽڵĵ�ַ.	
	NRF24L01_Read_Buf(TX_ADDR,buf,5); //����д��ĵ�ַ  
	for(i=0;i<5;i++)if(buf[i]!=0XA5)break;	 							   
	if(i!=5)return 1;//���24L01����	
	return 0;		 //��⵽24L01
}	

//SPIд�Ĵ���
//reg:ָ���Ĵ�����ַ
//value:д���ֵ
uint8_t NRF24L01_Write_Reg(uint8_t reg,uint8_t value)
{
	uint8_t status;	
	HAL_GPIO_WritePin(GPIOC, CSN_NRF_Pin, GPIO_PIN_RESET);//ʹ��24L01
	rt_hw_us_delay(8);
	status = HAL_SPI_Transmit(&hspi1,(uint8_t *)&reg,1,10000);//���ͼĴ����� 
	status = HAL_SPI_Transmit(&hspi1,(uint8_t *)&value,1,10000);//д��Ĵ�����ֵ 
	HAL_GPIO_WritePin(GPIOC, CSN_NRF_Pin, GPIO_PIN_SET);//SPIƬѡȡ��
	rt_hw_us_delay(8);
	return(status);       			//����״ֵ̬
}
//��ȡSPI�Ĵ���ֵ
//reg:Ҫ���ļĴ���
uint8_t NRF24L01_Read_Reg(uint8_t reg)
{
	uint8_t status,reg_val;	    
 	HAL_GPIO_WritePin(GPIOC, CSN_NRF_Pin, GPIO_PIN_RESET);//ʹ��24L01
 	rt_hw_us_delay(8);
	status = HAL_SPI_Transmit(&hspi1,(uint8_t *)&reg,1,10000);//���ͼĴ����� 
	status = HAL_SPI_Receive(&hspi1,(uint8_t *)&reg_val,1,10000);//��ȡ�Ĵ�������
	HAL_GPIO_WritePin(GPIOC, CSN_NRF_Pin, GPIO_PIN_SET);//SPIƬѡȡ��	
  rt_hw_us_delay(8);
	if(status)
		return status;		//���ش���ֵ
  else
		return(reg_val);           //���ض�ȡֵ
}	
//��ָ��λ�ö���ָ�����ȵ�����
//reg:�Ĵ���(λ��)
//*pBuf:����ָ��
//len:���ݳ���
//����ֵ,�˴ζ�����״̬�Ĵ���ֵ 
uint8_t NRF24L01_Read_Buf(uint8_t reg,uint8_t *pBuf,uint8_t len)
{
	uint8_t status;	       
	HAL_GPIO_WritePin(GPIOC, CSN_NRF_Pin, GPIO_PIN_RESET);//ʹ��24L01
	rt_hw_us_delay(8);
	status = HAL_SPI_Transmit(&hspi1,(uint8_t *)&reg,1,10000);//���ͼĴ����� 
	status = HAL_SPI_Receive(&hspi1,pBuf,len,10000);//��ȡ�Ĵ�������
	HAL_GPIO_WritePin(GPIOC, CSN_NRF_Pin, GPIO_PIN_SET);//SPIƬѡȡ��	delay_us(8);
	return status;        //���ض�����״ֵ̬
}
//��ָ��λ��дָ�����ȵ�����
//reg:�Ĵ���(λ��)
//*pBuf:����ָ��
//len:���ݳ���
//����ֵ,�˴ζ�����״̬�Ĵ���ֵ
uint8_t NRF24L01_Write_Buf(uint8_t reg, uint8_t *pBuf, uint8_t len)
{
	uint8_t status;	    
 	HAL_GPIO_WritePin(GPIOC, CSN_NRF_Pin, GPIO_PIN_RESET);//ʹ��24L01
 	rt_hw_us_delay(8);
  status = HAL_SPI_Transmit(&hspi1,(uint8_t *)&reg,1,10000);//���ͼĴ����� 
	status = HAL_SPI_Transmit(&hspi1,pBuf,len,10000);//���ͼĴ����� 
	HAL_GPIO_WritePin(GPIOC, CSN_NRF_Pin, GPIO_PIN_SET);//SPIƬѡȡ��	delay_us(8);delay_us(8);
  return status;          //���ض�����״ֵ̬
}				   
//����NRF24L01����һ������
//txbuf:�����������׵�ַ
//����ֵ:�������״��
uint8_t NRF24L01_TxPacket(uint8_t *txbuf)
{
	uint8_t sta;
 	HAL_GPIO_WritePin(GPIOC, CE_NRF_Pin, GPIO_PIN_RESET);
	rt_hw_us_delay(8);
  NRF24L01_Write_Buf(WR_TX_PLOAD,txbuf,TX_PLOAD_WIDTH);//д���ݵ�TX BUF  32���ֽ�
 	HAL_GPIO_WritePin(GPIOC, CE_NRF_Pin, GPIO_PIN_SET);//��������	   
 	rt_hw_us_delay(8);
	while(HAL_GPIO_ReadPin(IRQ_NRF_GPIO_Port,IRQ_NRF_Pin)!=0);//�ȴ��������
	sta=NRF24L01_Read_Reg(STATUS);  //��ȡ״̬�Ĵ�����ֵ	   
	NRF24L01_Write_Reg(NRF_WRITE_REG+STATUS,sta); //���TX_DS��MAX_RT�жϱ�־
	if(sta&MAX_TX)//�ﵽ����ط�����
	{
		NRF24L01_Write_Reg(FLUSH_TX,0xff);//���TX FIFO�Ĵ��� 
		//sta=NRF24L01_Read_Reg(STATUS);  //��ȡ״̬�Ĵ�����ֵ	 
		return MAX_TX; 
	}
	if(sta&TX_OK)//�������
	{
		return TX_OK;
	}
	return 0xff;//����ԭ����ʧ��
}
//����NRF24L01����һ������
//txbuf:�����������׵�ַ
//����ֵ:0��������ɣ��������������
uint8_t NRF24L01_RxPacket(uint8_t *rxbuf)
{
	uint8_t sta;		    							   
	sta=NRF24L01_Read_Reg(STATUS);  //��ȡ״̬�Ĵ�����ֵ    	 
	if(sta&RX_OK)//���յ�����
	{
		NRF24L01_Read_Buf(RD_RX_PLOAD,rxbuf,RX_PLOAD_WIDTH);//��ȡ����
		NRF24L01_Write_Reg(FLUSH_RX,0xff);//���RX FIFO�Ĵ��� 
		NRF24L01_Write_Reg(NRF_WRITE_REG+STATUS,sta); //���TX_DS��MAX_RT�жϱ�־
		return 0; 
	}	
	return 1;//û�յ��κ�����
}


void NRF24L01_ModeInt(void)
{	
	MYADDR = 0;
	HAL_GPIO_WritePin(GPIOC, CE_NRF_Pin, GPIO_PIN_RESET);//  
	rt_hw_us_delay(8);
  	NRF24L01_Write_Buf(NRF_WRITE_REG+RX_ADDR_P0,(uint8_t*)RX0_ADDRESS,RX_ADR_WIDTH);//дRX�ڵ��ַ
		NRF24L01_Write_Buf(NRF_WRITE_REG+RX_ADDR_P1,(uint8_t*)RX1_ADDRESS,RX_ADR_WIDTH);//дRX1�ڵ��ַ

		NRF24L01_Write_Reg(NRF_WRITE_REG+RX_ADDR_P2,0x20);
		NRF24L01_Write_Reg(NRF_WRITE_REG+RX_ADDR_P3,0x30);
		NRF24L01_Write_Reg(NRF_WRITE_REG+RX_ADDR_P4,0x40);
		NRF24L01_Write_Reg(NRF_WRITE_REG+RX_ADDR_P5,0x50);
	
  	NRF24L01_Write_Reg(NRF_WRITE_REG+EN_AA,0x3F);    	//ʹ��ͨ��0-5���Զ�Ӧ��  ,��ֹ�Զ�Ӧ��  
  	NRF24L01_Write_Reg(NRF_WRITE_REG+EN_RXADDR,0x3F);	//ʹ��ͨ��0-5�Ľ�������  	 
  	NRF24L01_Write_Reg(NRF_WRITE_REG+RF_CH,((MYADDR &0x0f) + 0x20));	    	//����RFͨ��Ƶ��		
	
		NRF24L01_Write_Reg(NRF_WRITE_REG+SETUP_RETR,0x07);//�����Զ��ط����ʱ��:250us + 86us;����Զ��ط�����:16��f����	
  	NRF24L01_Write_Reg(NRF_WRITE_REG+RX_PW_P0,RX_PLOAD_WIDTH);//ѡ��ͨ��0����Ч���ݿ�� 	
		NRF24L01_Write_Reg(NRF_WRITE_REG+RX_PW_P1,RX_PLOAD_WIDTH);//ѡ��ͨ��1����Ч���ݿ�� 
		NRF24L01_Write_Reg(NRF_WRITE_REG+RX_PW_P2,RX_PLOAD_WIDTH);//ѡ��ͨ��2����Ч���ݿ�� 
		NRF24L01_Write_Reg(NRF_WRITE_REG+RX_PW_P3,RX_PLOAD_WIDTH);//ѡ��ͨ��3����Ч���ݿ�� 
		NRF24L01_Write_Reg(NRF_WRITE_REG+RX_PW_P4,RX_PLOAD_WIDTH);//ѡ��ͨ��4����Ч���ݿ�� 
		NRF24L01_Write_Reg(NRF_WRITE_REG+RX_PW_P5,RX_PLOAD_WIDTH);//ѡ��ͨ��5����Ч���ݿ�� 
	
  	NRF24L01_Write_Reg(NRF_WRITE_REG+RF_SETUP,0x0f);	//����TX�������,0db����,2Mbps,���������濪��   
	
		NRF24L01_Write_Reg(NRF_WRITE_REG+CONFIG, 0x0f);		//���û�������ģʽ�Ĳ���;PWR_UP,EN_CRC,16BIT_CRC,����ģʽ 
  	HAL_GPIO_WritePin(GPIOC, CE_NRF_Pin, GPIO_PIN_SET);//	 //CEΪ��,�������ģʽ 
  rt_hw_us_delay(8);
}

//�ú�����ʼ��NRF24L01��RXģʽ
//����RX��ַ,дRX���ݿ��,ѡ��RFƵ��,�����ʺ�LNA HCURR
//��CE��ߺ�,������RXģʽ,�����Խ���������		   
void NRF24L01_RX_Mode(void)
{
	HAL_GPIO_WritePin(GPIOC, CE_NRF_Pin, GPIO_PIN_RESET);
	rt_hw_us_delay(8);
  	NRF24L01_Write_Buf(NRF_WRITE_REG+RX_ADDR_P0,(uint8_t*)RX0_ADDRESS,RX_ADR_WIDTH);//дRX�ڵ��ַ
	  NRF24L01_Write_Reg(NRF_WRITE_REG+CONFIG, 0x0f);		//���û�������ģʽ�Ĳ���;PWR_UP,EN_CRC,16BIT_CRC,����ģʽ 
	
  HAL_GPIO_WritePin(GPIOC, CE_NRF_Pin, GPIO_PIN_SET);//ʹ��24L01  
  rt_hw_us_delay(8);
}						 
//�ú�����ʼ��NRF24L01��TXģʽ
//����TX��ַ,дTX���ݿ��,����RX�Զ�Ӧ��ĵ�ַ,���TX��������,ѡ��RFƵ��,�����ʺ�LNA HCURR
//PWR_UP,CRCʹ��
//��CE��ߺ�,������RXģʽ,�����Խ���������		   
//CEΪ�ߴ���10us,����������.	 
void NRF24L01_TX_Mode(void)
{														 
	HAL_GPIO_WritePin(GPIOC, CE_NRF_Pin, GPIO_PIN_RESET);
	rt_hw_us_delay(8);	
	NRF24L01_Write_Buf(NRF_WRITE_REG+TX_ADDR,(uint8_t*)TX_ADDRESS,TX_ADR_WIDTH);//дTX�ڵ��ַ 
  	NRF24L01_Write_Buf(NRF_WRITE_REG+RX_ADDR_P0,(uint8_t*)TX_ADDRESS,TX_ADR_WIDTH); //����TX�ڵ��ַ,��ҪΪ��ʹ��ACK	  
	NRF24L01_Write_Reg(NRF_WRITE_REG+CONFIG,0x0e);    //���û�������ģʽ�Ĳ���;PWR_UP,EN_CRC,16BIT_CRC,����ģʽ,���������ж�
	HAL_GPIO_WritePin(GPIOC, CE_NRF_Pin, GPIO_PIN_SET);//ʹ��24L01  //CEΪ��,10us����������
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
		//ͨ��nrf24l01����
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

