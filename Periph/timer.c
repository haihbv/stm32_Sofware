#include "timer.h"

void TIMER_Init(TIM_TypeDef *TIMx, TIM_ConfigTypeDef *TIM_ConfigStruct)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	NVIC_InitTypeDef				NVIC_InitStruct;
	
	if (TIMx == TIM1)
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
		NVIC_InitStruct.NVIC_IRQChannel = TIM1_UP_IRQn;
	}
	else if (TIMx == TIM2)
	{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
		NVIC_InitStruct.NVIC_IRQChannel = TIM2_IRQn;
	}
	else if (TIMx == TIM3)
	{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
		NVIC_InitStruct.NVIC_IRQChannel = TIM3_IRQn;
	}
	else
	{
		return;
	}
	TIM_TimeBaseInitStruct.TIM_Prescaler = TIM_ConfigStruct->Prescaler;
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Period = TIM_ConfigStruct->Period;
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIMx, &TIM_TimeBaseInitStruct);
	
	if (TIM_ConfigStruct->Interrupt == ENABLE)
	{
		TIM_ClearFlag(TIMx, TIM_FLAG_Update);
		TIM_ITConfig(TIMx, TIM_IT_Update, ENABLE);
		
		NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
		NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
		NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
		NVIC_Init(&NVIC_InitStruct);
	}
	
}
void TIMER_Start(TIM_TypeDef *TIMx)
{
	TIM_Cmd(TIMx, ENABLE);
	if (TIMx == TIM1)
	{
		TIM_CtrlPWMOutputs(TIM1, ENABLE);
	}
}
void TIMER_Stop(TIM_TypeDef *TIMx)
{
	TIM_Cmd(TIMx, DISABLE);
	if (TIMx == TIM1)
	{
		TIM_CtrlPWMOutputs(TIM1, DISABLE);
	}
}

void TIMER_PWM_Init(TIM_TypeDef *TIMx, TIM_PWMConfigTypeDef *TIM_PWMConfigStruct)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_OCInitTypeDef 			TIM_OCInitStruct;
	
	if (TIMx == TIM1)
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	}
	else if (TIMx == TIM2)
	{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	}
	else if (TIMx == TIM3)
	{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	}
	else
	{
		return;
	}
	
	TIM_TimeBaseStructInit(&TIM_TimeBaseInitStruct);
	TIM_TimeBaseInitStruct.TIM_Prescaler = TIM_PWMConfigStruct->Prescaler;
	TIM_TimeBaseInitStruct.TIM_Period = TIM_PWMConfigStruct->Period;
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIMx, &TIM_TimeBaseInitStruct);
	
	TIM_OCStructInit(&TIM_OCInitStruct);
	TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStruct.TIM_Pulse = TIM_PWMConfigStruct->Pulse;
	TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;
	
	switch(TIM_PWMConfigStruct->Channel)
	{
		case 1:
			TIM_OC1Init(TIMx, &TIM_OCInitStruct);
			TIM_OC1PreloadConfig(TIMx, TIM_OCPreload_Enable);
			break;
		case 2:
			TIM_OC2Init(TIMx, &TIM_OCInitStruct);
			TIM_OC2PreloadConfig(TIMx, TIM_OCPreload_Enable);
			break;
		case 3:
			TIM_OC3Init(TIMx, &TIM_OCInitStruct);
			TIM_OC3PreloadConfig(TIMx, TIM_OCPreload_Enable);
			break;
		case 4:
			TIM_OC4Init(TIMx, &TIM_OCInitStruct);
			TIM_OC4PreloadConfig(TIMx, TIM_OCPreload_Enable);
			break;
		default:
			return;
	}
	
	/* Enable ARR preload */
	TIM_ARRPreloadConfig(TIMx, ENABLE);
}

/**************************************************************
* Delay - API
***************************************************************/
static void TIM1_Delay(uint32_t msDelay)
{
	for (volatile uint32_t i = 0; i < msDelay; i++)
	{
		TIM_SetCounter(TIM1, 0);
		while (TIM_GetFlagStatus(TIM1, TIM_FLAG_Update) == RESET);
		TIM_ClearFlag(TIM1, TIM_FLAG_Update);
	}
}

static void TIM2_Delay(uint32_t msDelay)
{
	for (volatile uint32_t i = 0; i < msDelay; i++)
	{
		TIM_SetCounter(TIM2, 0);
		while (TIM_GetFlagStatus(TIM2, TIM_FLAG_Update) == RESET);
		TIM_ClearFlag(TIM2, TIM_FLAG_Update);
	}
}

static void TIM3_Delay(uint32_t msDelay)
{
	for (volatile uint32_t i = 0; i < msDelay; i++)
	{
		TIM_SetCounter(TIM3, 0);
		while (TIM_GetFlagStatus(TIM3, TIM_FLAG_Update) == RESET);
		TIM_ClearFlag(TIM3, TIM_FLAG_Update);
	}
}

TIM_Delay_t TIMER1 = { TIM1_Delay };
TIM_Delay_t TIMER2 = { TIM2_Delay };
TIM_Delay_t TIMER3 = { TIM3_Delay };

void TIMER_PWMLed(TIM_TypeDef *TIMx)
{
	static int16_t duty = 0;
	static int16_t step = 25;
	
	duty += step;
	if (duty >= 1000)
	{
		duty = 1000;
		step = -step;
	}
	else if (duty <= 0)
	{
		duty = 0;
		step = -step;
	}
	TIM_SetCompare4(TIMx, (uint16_t)duty);
}

void TIM2_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
		// GPIOC->ODR ^= GPIO_Pin_13;
	}
}
