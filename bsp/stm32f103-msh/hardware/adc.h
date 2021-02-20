#ifndef __ADC_H
#define __ADC_H	
#include "stm32f1xx_hal.h"
#include <rtthread.h>
#include <rtdef.h>
#include <rthw.h>


void Adc_Init(void);
uint16_t Get_Adc(void);
int thread_adc_init(void);

//u16 Get_Adc_Average(u8 ch,u8 times); 
//u16 Get_Adc_Value(void);
#endif 
