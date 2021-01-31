 #include "adc.h"
 
 ADC_HandleTypeDef hadc1;
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//adc��������			   
//������̳:
//�޸�����:
//�汾��V1.0
////////////////////////////////////////////////////////////////////////////////// 	  
extern void Error_Handler(void);
/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static int MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */
  GPIO_InitTypeDef GPIO_InitStruct = {0};
	__HAL_RCC_GPIOA_CLK_ENABLE();
	
	GPIO_InitStruct.Pin = GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */
  /** Common config 
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel 
  */
  sConfig.Channel = ADC_CHANNEL_4;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_28CYCLES_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */
  return 0;
  /* USER CODE END ADC1_Init 2 */
}
INIT_BOARD_EXPORT(MX_ADC1_Init);

//���ADCֵ
uint16_t Get_Adc(void)
{
	uint16_t temp_adv;
	HAL_ADC_Start(&hadc1);
	HAL_ADC_PollForConversion(&hadc1,1000);
	temp_adv = HAL_ADC_GetValue(&hadc1);
	HAL_ADC_Stop(&hadc1);
	return temp_adv;
}

#define ADC_THREAD_PRIORITY         (RT_THREAD_PRIORITY_MAX - 4)
#define ADC_THREAD_STACK_SIZE       512
#define ADC_THREAD_TIMESLICE        100

ALIGN(RT_ALIGN_SIZE)
static char thd_adc_stack[ADC_THREAD_STACK_SIZE];
static struct rt_thread thd_adc;

#include "cd4051.h"
static void ADCread_scan_entey(void *parameter)
{
	uint16_t distance;
	while(1)
	{
		
		rt_thread_mdelay(200);
	}
}

int thread_adc_init(void)
{
	rt_thread_init(&thd_adc,
								 "thd_adc",
								 ADCread_scan_entey,
								 RT_NULL,
								 &thd_adc_stack[0],
								 sizeof(thd_adc_stack),
								 ADC_THREAD_PRIORITY - 1, ADC_THREAD_TIMESLICE);
										 
	rt_thread_startup(&thd_adc);
	return 0;
}

//��ʼ��ADC
//�������ǽ��Թ���ͨ��Ϊ��
//����Ĭ�Ͻ�����ͨ��0~3																	   
//void  Adc_Init(void)
//{ 	
//	ADC_InitTypeDef ADC_InitStructure; 
//	GPIO_InitTypeDef GPIO_InitStructure;
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |RCC_APB2Periph_ADC1	, ENABLE );	  //ʹ��ADC1ͨ��ʱ��
//	RCC_ADCCLKConfig(RCC_PCLK2_Div6);   //����ADC��Ƶ����6 72M/6=12,ADC���ʱ�䲻�ܳ���14M
//	
//	//PA2 ��Ϊģ��ͨ����������                         
//	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_2;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		//ģ����������
//	GPIO_Init(GPIOA, &GPIO_InitStructure);	
//	
//	
//	ADC_DeInit(ADC1);  //��λADC1,������ ADC1 ��ȫ���Ĵ�������Ϊȱʡֵ

//	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//ADC����ģʽ:ADC1��ADC2�����ڶ���ģʽ
//	ADC_InitStructure.ADC_ScanConvMode = DISABLE;	//ģ��ת�������ڵ�ͨ��ģʽ
//	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;	//ģ��ת�������ڵ���ת��ģʽ
//	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//ת��������������ⲿ��������
//	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//ADC�����Ҷ���
//	ADC_InitStructure.ADC_NbrOfChannel = 1;	//˳����й���ת����ADCͨ������Ŀ
//	ADC_Init(ADC1, &ADC_InitStructure);	//����ADC_InitStruct��ָ���Ĳ�����ʼ������ADCx�ļĴ���   

//  ADC_TempSensorVrefintCmd(ENABLE);//�ڲ��¶ȴ���������
//	
//	ADC_Cmd(ADC1, ENABLE);	//ʹ��ָ����ADC1
//	
//	ADC_ResetCalibration(ADC1);	//ʹ�ܸ�λУ׼  
//	 
//	while(ADC_GetResetCalibrationStatus(ADC1));	//�ȴ���λУ׼����
//	
//	ADC_StartCalibration(ADC1);	 //����ADУ׼
// 
//	while(ADC_GetCalibrationStatus(ADC1));	 //�ȴ�У׼����
// 
////	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//ʹ��ָ����ADC1�����ת����������

//}				  
//���ADCֵ
//ch:ͨ��ֵ 0~3
//u16 Get_Adc(u8 ch)   
//{
//  	//����ָ��ADC�Ĺ�����ͨ����һ�����У�����ʱ��
//	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_28Cycles5 );	//ADC1,ADCͨ��,����ʱ��Ϊ239.5����	  			    
//  
//	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//ʹ��ָ����ADC1�����ת����������	
//	 
//	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//�ȴ�ת������

//	return ADC_GetConversionValue(ADC1);	//�������һ��ADC1�������ת�����
//}

//u16 Get_Adc_Value(void)
//{
//	return Get_Adc(2);
//}
//ð������
//void Bubble_Sort(u16 *num, u16 n)
//{
//    u16 i, j, tp;
//    for(i = 0; i < n; i++)
//    {
//        for(j = 0; i + j < n - 1; j++)
//        {
//            if(num[j] > num[j + 1])
//            {
//                tp = num[j];
//                num[j] = num[j + 1];
//                num[j + 1] = tp;
//            }
//        }
//    }
//}

//u16 Get_Adc_Average(u8 ch,u8 times)
//{
//	u32 temp_val=0;
//	u16 temp[15];
//	u8 t,no;
//	for(t=0;t<times;t++)
//	{
//		temp[t]=Get_Adc(ch);
//		delay_us(200);
//	}
//	Bubble_Sort(temp,times);
//	for(t=0;t<times;t++)
//	{
//		temp[t]=Get_Adc(ch);
//	}
//	no = times>>1;
//	if(no>1)
//		temp_val = temp[no-1] + temp[no] + temp[no+1];
//	else
//		return temp_val = temp[no];
//	return temp_val/3;
//} 

//u16 Get_Adc_Average(u8 ch,u8 times)
//{
//	u32 temp_val=0;
//	u8 t;
//	for(t=0;t<times;t++)
//	{
//		temp_val+=Get_Adc(ch);
//		//delay_ms(1);
//	}
//	return temp_val/times;
//} 	 



























