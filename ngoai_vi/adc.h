#ifndef __ADC_H
#define __ADC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f10x.h"        

typedef struct
{
	void 			(*Init)(uint8_t Channel);
	uint16_t 	(*Read)(uint8_t Channel);
} ADC_Driver_t;

extern ADC_Driver_t hADC1;
extern ADC_Driver_t hADC2;

void ADC_Start(ADC_TypeDef *ADCx);
void ADC_Stop(ADC_TypeDef *ADCx);
void ADC_Calibration(ADC_TypeDef *ADCx);

#ifdef __cplusplus
}
#endif

#endif // __ADC_H
