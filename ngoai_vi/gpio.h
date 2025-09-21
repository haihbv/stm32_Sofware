#ifndef __GPIO_H
#define __GPIO_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f10x.h"
#include "stm32f10x_gpio.h"

typedef enum
{
	false = 0,
	true = !false
} bool;

/**
 * @brief   Khoi tao chan GPIO
 * @param   pGPIOx: GPIOA, GPIOB, GPIOC, ...
 * @param   pin: GPIO_Pin_x
 * @param   mode: GPIO_Mode_xxx
 * @param   speed: GPIO_Speed_xxx
 */
void GPIOx_Init(GPIO_TypeDef *pGPIOx, uint16_t pin, GPIOMode_TypeDef mode, GPIOSpeed_TypeDef speed);

/**
 * @brief   Khoi tao nut nhan
 * @param   pGPIOx: GPIOA, GPIOB, GPIOC, ...
 * @param   pin: GPIO_Pin_x
 */
void BTN_Init(GPIO_TypeDef *pGPIOx, uint16_t pin);

#ifdef __cplusplus
}
#endif

#endif // __GPIO_H
