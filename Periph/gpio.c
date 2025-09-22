#include "gpio.h"
#include "delay.h"

void GPIOx_Init(GPIO_TypeDef *pGPIOx, uint16_t pin, GPIOMode_TypeDef mode, GPIOSpeed_TypeDef speed)
{
	if (pGPIOx == GPIOA)
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	}
	else if (pGPIOx == GPIOB)
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	}
	else if (pGPIOx == GPIOC)
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	}
	else
	{
		return;
	}
	
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.GPIO_Pin = pin;
	GPIO_InitStruct.GPIO_Mode = mode;
	GPIO_InitStruct.GPIO_Speed = speed;
	GPIO_Init(pGPIOx, &GPIO_InitStruct);
}

void BTN_Init(GPIO_TypeDef *pGPIOx, uint16_t pin)
{
	GPIOx_Init(pGPIOx, pin, GPIO_Mode_IPU, GPIO_Speed_50MHz);
}
