#ifndef __I2C_H
#define __I2C_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f10x.h"
#include "stm32f10x_i2c.h"

typedef struct
{
	void 		(*Init)(void);
	void 		(*Start)(void);
	void 		(*Stop)(void);
	uint8_t (*Master_Transmit)(uint8_t DevAddress, uint8_t *pData, uint16_t Size);
	uint8_t (*Master_Receive)(uint8_t DevAddress, uint8_t *pData, uint16_t Size);
} I2C_Driver_t;

extern I2C_Driver_t hI2C1;
extern I2C_Driver_t hI2C2;

void I2Cx_Init(I2C_TypeDef *I2Cx);
void I2C_Start(I2C_TypeDef *I2Cx);
void I2C_Stop(I2C_TypeDef *I2Cx);
void I2C_WriteAddr(I2C_TypeDef *I2Cx, uint8_t address, uint8_t isRead);
uint8_t I2C_WriteData(I2C_TypeDef *I2Cx, uint8_t Data);
uint8_t I2C_ReadData(I2C_TypeDef *I2Cx, uint8_t ack);

#ifdef __cplusplus
}
#endif

#endif // __I2C_H
