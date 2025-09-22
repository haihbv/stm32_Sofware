#include "spi.h"

SPI_Driver_t hSPI1;
SPI_Driver_t hSPI2;

void SPI_Start(SPI_TypeDef *SPIx)
{
	SPI_Cmd(SPIx, ENABLE);
}

void SPI_Stop(SPI_TypeDef *SPIx)
{
	SPI_Cmd(SPIx, DISABLE);
}

void SPIx_Init(SPI_TypeDef *SPIx, uint16_t SPI_BaudRatePrescaler, uint16_t SPI_DataSize)
{
	GPIO_InitTypeDef 	GPIO_InitStruct;
	SPI_InitTypeDef		SPI_InitStruct;
	if (SPIx == SPI1)
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_SPI1 | RCC_APB2Periph_GPIOC, ENABLE);
		
		// MOSI - SCK
		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA, &GPIO_InitStruct);
		
		// MISO
		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		GPIO_Init(GPIOA, &GPIO_InitStruct);
		
		// CS
		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_15;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_Init(GPIOC, &GPIO_InitStruct);
		GPIO_SetBits(GPIOC, GPIO_Pin_15);
		
		hSPI1.CS_Port = GPIOC;
		hSPI1.CS_Pin = GPIO_Pin_15;
	}
	else if (SPIx == SPI2)
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
		
		// MOSI - SCK
		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_15;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOB, &GPIO_InitStruct);
		
		// MISO
		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_14;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		GPIO_Init(GPIOB, &GPIO_InitStruct);
		
		// CS
		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_Init(GPIOB, &GPIO_InitStruct);
		GPIO_SetBits(GPIOB, GPIO_Pin_12);
		
		hSPI2.CS_Port = GPIOB;
		hSPI2.CS_Pin = GPIO_Pin_12;
	}
	else 
	{
		return;
	}
	SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStruct.SPI_Mode = SPI_Mode_Master;
	SPI_InitStruct.SPI_DataSize = SPI_DataSize;
	SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler;
	SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStruct.SPI_CRCPolynomial = 7;
	SPI_Init(SPIx, &SPI_InitStruct);
	
	SPI_Start(SPIx);
}

uint8_t SPI_TR_Data(SPI_TypeDef *SPIx, uint8_t Data)
{
	while (SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_TXE) == RESET);
	SPI_I2S_SendData(SPIx, Data);
	while (SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_RXNE) == RESET);
	return (uint8_t)SPI_I2S_ReceiveData(SPIx);
}

static inline void SPI1_CS_Low(void)
{
	hSPI1.CS_Port->BRR = hSPI1.CS_Pin;
}
static inline void SPI1_CS_High(void)
{
	hSPI1.CS_Port->BSRR = hSPI1.CS_Pin;
}
static inline void SPI2_CS_Low(void)
{
	hSPI2.CS_Port->BRR = hSPI2.CS_Pin;
}
static inline void SPI2_CS_High(void)
{
	hSPI2.CS_Port->BSRR = hSPI2.CS_Pin;
}

static inline uint8_t SPI1_Transfer(uint8_t Data)
{
	return SPI_TR_Data(SPI1, Data);
}

static inline uint8_t SPI2_Transfer(uint8_t Data)
{
	return SPI_TR_Data(SPI2, Data);
}

static inline void SPI1_Init(uint16_t SPI_BaudRatePrescaler, uint16_t SPI_DataSize)
{
	SPIx_Init(SPI1, SPI_BaudRatePrescaler, SPI_DataSize);
}

static inline void SPI2_Init(uint16_t SPI_BaudRatePrescaler, uint16_t SPI_DataSize)
{
	SPIx_Init(SPI2, SPI_BaudRatePrescaler, SPI_DataSize);
}

void SPI_AutoInit(void) __attribute__((constructor));
void SPI_AutoInit(void)
{
	hSPI1.Init 			= SPI1_Init;
	hSPI1.CS_Low 		= SPI1_CS_Low;
	hSPI1.CS_High 	= SPI1_CS_High;
	hSPI1.Transfer 	= SPI1_Transfer;
	
	hSPI2.Init 			= SPI2_Init;
	hSPI2.CS_Low 		= SPI2_CS_Low;
	hSPI2.CS_High 	= SPI2_CS_High;
	hSPI2.Transfer 	= SPI2_Transfer;	
}
