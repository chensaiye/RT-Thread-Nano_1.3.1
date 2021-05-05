 #include "adc.h"
 
 ADC_HandleTypeDef hadc1;
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//adc驱动代码			   
//技术论坛:
//修改日期:
//版本：V1.0
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
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */
  //HAL_ADC_DeInit(&hadc1);
	__HAL_RCC_ADC1_CLK_ENABLE();
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

uint16_t temp_adv;
//获得ADC值
uint16_t Get_Adc(void)
{
	HAL_ADC_Start(&hadc1);
	HAL_ADC_PollForConversion(&hadc1,1000);
	temp_adv = HAL_ADC_GetValue(&hadc1);
	//HAL_ADC_Stop(&hadc1);
	return temp_adv;
}

static uint16_t Get_ADC_Buf_Average(uint16_t len,uint16_t* buf)
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

#define ADC_THREAD_PRIORITY         (RT_THREAD_PRIORITY_MAX - 4)
#define ADC_THREAD_STACK_SIZE       512
#define ADC_THREAD_TIMESLICE        100

ALIGN(RT_ALIGN_SIZE)
static char thd_adc_stack[ADC_THREAD_STACK_SIZE];
static struct rt_thread thd_adc;

#include "cd4051.h"
#include "main_control.h"
extern union_status curr_status;//当前状态
extern pwm_value_union set_val;//设定值
extern pwm_value_union curr_val;//当前值
extern rt_sem_t sem_warning;

uint16_t ch = 0;
uint16_t numb = 0;
uint16_t ad_value[8];
uint16_t ad_group[8][5];
#define ADC_I_MIN  150
#define ADC_I_MAX  3000

//错误检测逻辑 --
//开灯情况下，PWM值达到设定值后开始采样，123通道为1组，456通道为一组，7通道为主灯
//副灯异常判断逻辑：当前通道的值与其他2个通道的平均值 差异超过25%，则通道错误标志置1，否则清零。

static void Error_Check(uint16_t avg,uint16_t diff,uint16_t ch)
{
	rt_sem_take(sem_warning,RT_WAITING_FOREVER);
	if((avg > ADC_I_MIN))//&&(avg < ADC_I_MAX))
	{
		if((ad_value[ch]>avg+diff)||((ad_value[ch]<avg-diff))||(ad_value[ch]>ADC_I_MAX))//||(ad_value[ch]<ADC_I_MIN))
			curr_status.value.error_Flag |= 0x01<<ch;
		else
			curr_status.value.error_Flag &= ~(0x01<<ch);
  }
	else
		curr_status.value.error_Flag &= ~(0x01<<ch);
	rt_sem_release(sem_warning);
}

uint8_t old_error_flag=0;
//LED驱动电压反馈值 异常判断
static void CH_Error_Test(void)
{
	uint16_t tp_value=0, diff_val=0;
	
	old_error_flag = curr_status.value.error_Flag;
	//通道1
	tp_value = (ad_value[1]+ad_value[2])>>1;//平均值
	diff_val = tp_value>>2;
	Error_Check(tp_value,diff_val,0);
	
	//通道2
	tp_value = (ad_value[0]+ad_value[2])>>1;//平均值
	diff_val = tp_value>>2;
	Error_Check(tp_value,diff_val,1);
	
	//通道3
	tp_value = (ad_value[0]+ad_value[1])>>1;//平均值
	diff_val = tp_value>>2;
	Error_Check(tp_value,diff_val,2);
	
	//通道4
	tp_value = (ad_value[4]+ad_value[5])>>1;//平均值
	diff_val = tp_value>>2;
	Error_Check(tp_value,diff_val,3);
	
	//通道5
	tp_value = (ad_value[3]+ad_value[5])>>1;//平均值
	diff_val = tp_value>>2;
	Error_Check(tp_value,diff_val,4);
	
	//通道6
	tp_value = (ad_value[3]+ad_value[4])>>1;//平均值
	diff_val = tp_value>>2;
	Error_Check(tp_value,diff_val,5);
	
	//通道7 //主灯异常判断逻辑：
	rt_sem_take(sem_warning,RT_WAITING_FOREVER);
	if((ad_value[6]>ADC_I_MAX))// ||(ad_value[6]<ADC_I_MIN))
		curr_status.value.error_Flag |= 0x40;
	else
		curr_status.value.error_Flag &= ~(0x40);
	rt_sem_release(sem_warning);
	if(old_error_flag != curr_status.value.error_Flag)
		led_manual_updata();
}


static void ADCread_scan_entey(void *parameter)
{
	while(1)
	{
		if(curr_status.value.pow_fg == ON)
		{
			if((set_val.buf[0]== curr_val.buf[0]) && (set_val.buf[1]== curr_val.buf[1]) && (set_val.buf[2]== curr_val.buf[2]))
			{
				//每通道采样5次
				for(numb=0;numb<5;numb++)
				{
					for(ch=0;ch<7;ch++)
					{
						CD4051_Ch_Select(ch);
						rt_thread_mdelay(100);
						ad_group[ch][numb] = Get_Adc();
					}
				}
				CD4051_Ch_Select(7);
				//取平均值
				for(ch=0;ch<7;ch++)
				{	
					ad_value[ch] = Get_ADC_Buf_Average(5,&(ad_group[ch][0]));
					if(ad_value[ch]>4095)
						ad_value[ch] = 4095;
				}
				CH_Error_Test();//LED驱动电压反馈值 异常判断
			}
			else
				curr_status.value.error_Flag &= 0x80;//clear all ch error
		}
		else
		{	
			curr_status.value.error_Flag &= 0x80;//clear all ch error
			for(ch=0;ch<7;ch++)
				ad_value[ch] = 0;
		}
		rt_thread_mdelay(250);
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

//初始化ADC
//这里我们仅以规则通道为例
//我们默认将开启通道0~3																	   
//void  Adc_Init(void)
//{ 	
//	ADC_InitTypeDef ADC_InitStructure; 
//	GPIO_InitTypeDef GPIO_InitStructure;
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |RCC_APB2Periph_ADC1	, ENABLE );	  //使能ADC1通道时钟
//	RCC_ADCCLKConfig(RCC_PCLK2_Div6);   //设置ADC分频因子6 72M/6=12,ADC最大时间不能超过14M
//	
//	//PA2 作为模拟通道输入引脚                         
//	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_2;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		//模拟输入引脚
//	GPIO_Init(GPIOA, &GPIO_InitStructure);	
//	
//	
//	ADC_DeInit(ADC1);  //复位ADC1,将外设 ADC1 的全部寄存器重设为缺省值

//	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//ADC工作模式:ADC1和ADC2工作在独立模式
//	ADC_InitStructure.ADC_ScanConvMode = DISABLE;	//模数转换工作在单通道模式
//	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;	//模数转换工作在单次转换模式
//	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//转换由软件而不是外部触发启动
//	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//ADC数据右对齐
//	ADC_InitStructure.ADC_NbrOfChannel = 1;	//顺序进行规则转换的ADC通道的数目
//	ADC_Init(ADC1, &ADC_InitStructure);	//根据ADC_InitStruct中指定的参数初始化外设ADCx的寄存器   

//  ADC_TempSensorVrefintCmd(ENABLE);//内部温度传感器开启
//	
//	ADC_Cmd(ADC1, ENABLE);	//使能指定的ADC1
//	
//	ADC_ResetCalibration(ADC1);	//使能复位校准  
//	 
//	while(ADC_GetResetCalibrationStatus(ADC1));	//等待复位校准结束
//	
//	ADC_StartCalibration(ADC1);	 //开启AD校准
// 
//	while(ADC_GetCalibrationStatus(ADC1));	 //等待校准结束
// 
////	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//使能指定的ADC1的软件转换启动功能

//}				  
//获得ADC值
//ch:通道值 0~3
//u16 Get_Adc(u8 ch)   
//{
//  	//设置指定ADC的规则组通道，一个序列，采样时间
//	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_28Cycles5 );	//ADC1,ADC通道,采样时间为239.5周期	  			    
//  
//	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//使能指定的ADC1的软件转换启动功能	
//	 
//	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//等待转换结束

//	return ADC_GetConversionValue(ADC1);	//返回最近一次ADC1规则组的转换结果
//}

//u16 Get_Adc_Value(void)
//{
//	return Get_Adc(2);
//}
//冒泡排序法
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



























