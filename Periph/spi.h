#ifndef __SPI_H
#define __SPI_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f10x.h"

typedef struct
{
	void 		(*Init)(uint16_t SPI_BaudRatePrescaler, uint16_t SPI_DataSize);
	void 		(*CS_Low)(void);
	void 		(*CS_High)(void);
	uint8_t	(*Transfer)(uint8_t Data);
	
	uint16_t				CS_Pin;
	GPIO_TypeDef 		*CS_Port;
} __attribute__((packed)) SPI_Driver_t;

extern SPI_Driver_t hSPI1;
extern SPI_Driver_t hSPI2;

void SPI_Start(SPI_TypeDef *SPIx);
void SPI_Stop(SPI_TypeDef *SPIx);
void SPIx_Init(SPI_TypeDef *SPIx, uint16_t SPI_BaudRatePrescaler, uint16_t SPI_DataSize);
uint8_t SPI_TR_Data(SPI_TypeDef *SPIx, uint8_t Data);

#ifdef __cplusplus
}
#endif

#endif // __SPI_H
