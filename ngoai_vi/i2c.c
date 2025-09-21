#include "i2c.h"

I2C_Driver_t hI2C1;
I2C_Driver_t hI2C2;

void I2Cx_Init(I2C_TypeDef *I2Cx)
{
	GPIO_InitTypeDef 	GPIO_InitStruct;
	I2C_InitTypeDef		I2C_InitStruct;
	if (I2Cx == I2C1)
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
		
		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_OD;
		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOB, &GPIO_InitStruct);
	}
	else if (I2Cx == I2C2)
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);
		
		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_OD;
		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOB, &GPIO_InitStruct);
	}
	else
	{
		return;
	}
	I2C_InitStruct.I2C_ClockSpeed = 100000;
	I2C_InitStruct.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStruct.I2C_OwnAddress1 = 0x00;
	I2C_InitStruct.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_Init(I2Cx, &I2C_InitStruct);
	I2C_Cmd(I2Cx, ENABLE);
}
void I2C_Start(I2C_TypeDef *I2Cx)
{
	I2C_GenerateSTART(I2Cx, ENABLE);
	while (I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT) == RESET);
}
void I2C_Stop(I2C_TypeDef *I2Cx)
{
	I2C_GenerateSTOP(I2Cx, ENABLE);
}
void I2C_WriteAddr(I2C_TypeDef *I2Cx, uint8_t address, uint8_t isRead)
{
	I2C_Send7bitAddress(I2Cx, (uint8_t)(address << 1), (isRead) ? I2C_Direction_Receiver : I2C_Direction_Transmitter);
	
	if (isRead)
	{
		while (I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED) == RESET);
	}
	else
	{
		while (I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) == RESET);
	}
}
uint8_t I2C_WriteData(I2C_TypeDef *I2Cx, uint8_t Data)
{
	I2C_SendData(I2Cx, Data);
	
	while (I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED) == RESET);
	if (I2C_GetFlagStatus(I2Cx, I2C_FLAG_AF) == SET)
	{
		I2C_ClearFlag(I2Cx, I2C_FLAG_AF);
		I2C_Stop(I2Cx);
		return 1; // error (nack)
	}
	return 0; // success
}
uint8_t I2C_ReadData(I2C_TypeDef *I2Cx, uint8_t ack)
{
	if (ack)
	{
		I2C_AcknowledgeConfig(I2Cx, ENABLE);
	}
	else
	{
		I2C_AcknowledgeConfig(I2Cx, DISABLE);
	}
	while (I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED) == RESET);
	return I2C_ReceiveData(I2Cx);
}
static uint8_t I2C_Master_Transmit(I2C_TypeDef *I2Cx, uint8_t DevAddress, uint8_t *pData, uint16_t Size)
{
	I2C_Start(I2Cx);
	I2C_WriteAddr(I2Cx, DevAddress, 0); // 0 = Write
	
	for (uint16_t i = 0; i < Size; i++)
	{
		if (I2C_WriteData(I2Cx, pData[i]))
		{
			I2C_Stop(I2Cx);
			return 1; // Error
		}
	}
	I2C_Stop(I2Cx);
	return 0; // Success
}

static uint8_t I2C_Master_Receive(I2C_TypeDef *I2Cx, uint8_t DevAddress, uint8_t *pData, uint16_t Size)
{
	I2C_Start(I2Cx);
	I2C_WriteAddr(I2Cx, DevAddress, 1); // 1 = Read
	
	for (uint16_t i = 0; i < Size; i++)
	{
		if (i == (Size - 1))
		{
			pData[i] = I2C_ReadData(I2Cx, 0); // end byte: NACK + Stop
		}
		else
		{
			pData[i] = I2C_ReadData(I2Cx, 1); // ACK
		}
	}
	
	I2C_Stop(I2Cx);
	return 0; // Success
}

static inline void I2C1_Init(void)
{
	I2Cx_Init(I2C1);
}

static inline void I2C2_Init(void)
{
	I2Cx_Init(I2C2);
}

static inline void I2C1_Start(void)
{
	I2C_Start(I2C1);
}
static inline void I2C2_Start(void)
{
	I2C_Start(I2C2);
}

static inline void I2C1_Stop(void)
{
	I2C_Stop(I2C1);
}
static inline void I2C2_Stop(void)
{
	I2C_Stop(I2C2);
}

static inline uint8_t I2C1_Master_Transmit(uint8_t DevAddress, uint8_t *pData, uint16_t Size)
{
	return I2C_Master_Transmit(I2C1, DevAddress, pData, Size);
}

static inline uint8_t I2C2_Master_Transmit(uint8_t DevAddress, uint8_t *pData, uint16_t Size)
{
	return I2C_Master_Transmit(I2C2, DevAddress, pData, Size);
}

static inline uint8_t I2C1_Master_Receive(uint8_t DevAddress, uint8_t *pData, uint16_t Size)
{
	return I2C_Master_Receive(I2C1, DevAddress, pData, Size);
}

static inline uint8_t I2C2_Master_Receive(uint8_t DevAddress, uint8_t *pData, uint16_t Size)
{
	return I2C_Master_Receive(I2C2, DevAddress, pData, Size);
}

void I2C_AutoInit(void) __attribute__((constructor));
void I2C_AutoInit(void)
{
	hI2C1.Init = I2C1_Init;
	hI2C1.Start = I2C1_Start;
	hI2C1.Stop = I2C1_Stop;
	hI2C1.Master_Transmit = I2C1_Master_Transmit;
	hI2C1.Master_Receive = I2C1_Master_Receive;
	
	hI2C2.Init = I2C2_Init;
	hI2C2.Start = I2C2_Start;
	hI2C2.Stop = I2C2_Stop;
	hI2C2.Master_Transmit = I2C2_Master_Transmit;
	hI2C2.Master_Receive = I2C2_Master_Receive;
}
