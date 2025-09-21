#ifndef TIMER_H_
#define TIMER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f10x.h"

/**
 * @brief  Timer base configuration
 */
typedef struct
{
	uint16_t Prescaler;
	uint16_t Period;
	FunctionalState Interrupt;
} __attribute__((packed)) TIM_ConfigTypeDef;

/**
 * @brief  Timer PWM configuration
 */
typedef struct
{
	uint16_t Prescaler;
	uint16_t Period;
	uint16_t Pulse; /* Duty cycle(CCR) = CCR / (ARR + 1) */
	uint8_t Channel;
} __attribute__((packed)) TIM_PWMConfigTypeDef;

void TIMER_Init(TIM_TypeDef *TIMx, TIM_ConfigTypeDef *TIM_InitStructure);
void TIMER_Start(TIM_TypeDef *TIMx);
void TIMER_Stop(TIM_TypeDef *TIMx);
void TIMER_PWM_Init(TIM_TypeDef *TIMx, TIM_PWMConfigTypeDef *TIM_PWMConfigStruct);

/**
 * @brief  Timer Delay configuration
 */
typedef struct
{
	void (*Delay)(uint32_t msDelay);
} TIM_Delay_t; 

/**************************************************************
* TIMER - API
***************************************************************/
extern TIM_Delay_t TIMER1;
extern TIM_Delay_t TIMER2;
extern TIM_Delay_t TIMER3;


/**
 * @brief  Timer PWM-Led configuration
 */
void TIMER_PWMLed(TIM_TypeDef *TIMx);

/**************************************************************
* TIMER - Interrupt 
***************************************************************/
void TIM1_UP_IRQHandler(void);
void TIM2_IRQHandler(void);
void TIM3_IRQHandler(void);

#ifdef __cplusplus
}
#endif

#endif /* TIMER_H_ */
