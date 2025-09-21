#include "adc.h"

ADC_Driver_t hADC1;
ADC_Driver_t hADC2;

void ADC_Start(ADC_TypeDef *ADCx)
{
	ADC_Cmd(ADCx, ENABLE);
}
void ADC_Stop(ADC_TypeDef *ADCx)
{
	ADC_Cmd(ADCx, DISABLE);
}


void ADC_Calibration(ADC_TypeDef *ADCx)
{
	ADC_ResetCalibration(ADCx);
	while (ADC_GetResetCalibrationStatus(ADCx) == SET);
	ADC_StartCalibration(ADCx);
	while (ADC_GetCalibrationStatus(ADCx) == SET);
}

/***********************************************************************************
* ADC - OOP
***********************************************************************************/

static void ADCx_Init(ADC_TypeDef *ADCx, uint8_t Channel)
{
	ADC_InitTypeDef 	ADC_InitStruct;
	GPIO_InitTypeDef 	GPIO_InitStruct;
	
	if (ADCx == ADC1)
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	}
	else if (ADCx == ADC2)
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2, ENABLE);
	}
	else
	{
		return;
	}
	
	if (Channel <= 7)
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
		GPIO_InitStruct.GPIO_Pin = (uint16_t)(1 << Channel);
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN;
		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA, &GPIO_InitStruct);
	}
	else if (Channel <= 9)
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
		GPIO_InitStruct.GPIO_Pin = (uint16_t)(1 << (Channel - 8));
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN;
		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOB, &GPIO_InitStruct);
	}
	else
	{
		return;
	}
	
	ADC_InitStruct.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStruct.ADC_ScanConvMode = DISABLE;
	ADC_InitStruct.ADC_ContinuousConvMode = DISABLE;
	ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStruct.ADC_NbrOfChannel = 1;
	ADC_Init(ADCx, &ADC_InitStruct);
	
	ADC_Start(ADCx);
	ADC_Calibration(ADCx);
}

static void ADC1_Init(uint8_t Channel)
{
	ADCx_Init(ADC1, Channel);
}

static void ADC2_Init(uint8_t Channel)
{
	ADCx_Init(ADC2, Channel);
}

static uint16_t ADC1_Read(uint8_t Channel)
{
	ADC_RegularChannelConfig(ADC1, Channel, 1, ADC_SampleTime_71Cycles5);
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
	while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
	return ADC_GetConversionValue(ADC1);
}

static uint16_t ADC2_Read(uint8_t Channel)
{
	ADC_RegularChannelConfig(ADC2, Channel, 1, ADC_SampleTime_71Cycles5);
	ADC_SoftwareStartConvCmd(ADC2, ENABLE);
	while (ADC_GetFlagStatus(ADC2, ADC_FLAG_EOC) == RESET);
	return ADC_GetConversionValue(ADC2);
}

void ADC_AutoInit(void) __attribute__((constructor));

void ADC_AutoInit(void)
{
	hADC1.Init = ADC1_Init;
	hADC1.Read = ADC1_Read;
	
	hADC2.Init = ADC2_Init;
	hADC2.Read = ADC2_Read;
}
