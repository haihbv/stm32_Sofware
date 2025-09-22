#ifndef __DELAY_H
#define __DELAY_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f10x.h"

void Delay_Init(void);
void DelayMs(uint32_t msDelay);
void DelayUs(uint32_t usDelay);

static inline uint32_t millis(void)
{
	extern volatile uint32_t tickMs;
	return tickMs;
}

void SysTick_Handler(void);

#ifdef __cplusplus
}
#endif

#endif //__DELAY_H
