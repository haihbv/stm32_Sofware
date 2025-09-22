#include "usart.h"
#include "delay.h"
#include <stdio.h>

USART_Driver_t hUSART1;
USART_Driver_t hUSART2;
USART_Driver_t hUSART3;

#define RX_BUF_SIZE 512
typedef struct
{
	volatile char buf[RX_BUF_SIZE];
	volatile uint16_t head;
	volatile uint16_t tail;
} RingBuffer_t;
static RingBuffer_t usart1_rx = {0};
static RingBuffer_t usart2_rx = {0};
static RingBuffer_t usart3_rx = {0};

void USARTx_Init(USART_TypeDef *USARTx, uint32_t USART_BaudRate)
{
	GPIO_InitTypeDef 		GPIO_InitStruct = {0};
	USART_InitTypeDef		USART_InitStruct = {0};
	NVIC_InitTypeDef		NVIC_InitStruct = {0};
	
	if (USARTx == USART1)
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);
		
		// TX 
		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA, &GPIO_InitStruct);
		
		// RX
		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		GPIO_Init(GPIOA, &GPIO_InitStruct);
		
		NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;
	}
	else if (USARTx == USART2)
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
		
		// TX 
		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA, &GPIO_InitStruct);
		
		// RX
		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		GPIO_Init(GPIOA, &GPIO_InitStruct);
		
		NVIC_InitStruct.NVIC_IRQChannel = USART2_IRQn;
	}
	else if (USARTx == USART3)
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
		
		// TX
		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOB, &GPIO_InitStruct);
		
		// RX
		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_11;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		GPIO_Init(GPIOB, &GPIO_InitStruct);
		
		NVIC_InitStruct.NVIC_IRQChannel = USART3_IRQn;
	}
	else 
	{
		return;
	}
	USART_InitStruct.USART_BaudRate = USART_BaudRate;
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;
	USART_InitStruct.USART_StopBits = USART_StopBits_1;
	USART_InitStruct.USART_Parity = USART_Parity_No;
	USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_Init(USARTx, &USART_InitStruct);
	USART_Cmd(USARTx, ENABLE);
	
	USART_ITConfig(USARTx, USART_IT_RXNE, ENABLE);
	
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_Init(&NVIC_InitStruct);
}

void USART_SendChar(USART_TypeDef *USARTx, char c)
{
	while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET);
	USART_SendData(USARTx, c);
}
void USART_SendStr(USART_TypeDef *USARTx, const char *str)
{
	while (*str)
	{
		USART_SendChar(USARTx, *str++);
	}
}

static inline int RingBuffer_Available(RingBuffer_t *rb)
{
	return (rb->head != rb->tail);
}

static inline char RingBuffer_Getchar(RingBuffer_t *rb)
{
	if (rb->head == rb->tail)
	{
		return 0;
	}
	char c = rb->buf[rb->tail];
	rb->tail = (rb->tail + 1) % RX_BUF_SIZE;
	return c;
}

static inline void RingBuffer_PutChar(RingBuffer_t *rb, char c)
{
	uint16_t next_head = (rb->head + 1) % RX_BUF_SIZE;
	if (next_head != rb->tail)
	{
		rb->buf[rb->head] = c;
		rb->head = next_head;
	}
}

/*****************************************************************
* Method
*****************************************************************/
static inline int USART1_Available(void)
{
	return RingBuffer_Available(&usart1_rx);
}
static inline char USART1_GetChar(void)
{
	return RingBuffer_Getchar(&usart1_rx);
}
static inline int USART2_Available(void)
{
	return RingBuffer_Available(&usart2_rx);
}
static inline char USART2_GetChar(void)
{
	return RingBuffer_Getchar(&usart2_rx);
}
static inline int USART3_Available(void)
{
	return RingBuffer_Available(&usart3_rx);
}
static inline char USART3_GetChar(void)
{
	return RingBuffer_Getchar(&usart3_rx);
}

static inline void USART1_Init(uint32_t USART_BaudRate)
{
	USARTx_Init(USART1, USART_BaudRate);
}
static inline void USART2_Init(uint32_t USART_BaudRate)
{
	USARTx_Init(USART2, USART_BaudRate);
}
static inline void USART3_Init(uint32_t USART_BaudRate)
{
	USARTx_Init(USART3, USART_BaudRate);
}

static inline void USART1_SendChar(char c)
{
	USART_SendChar(USART1, c);
}
static inline void USART2_SendChar(char c)
{
	USART_SendChar(USART2, c);
}
static inline void USART3_SendChar(char c)
{
	USART_SendChar(USART3, c);
}

static inline void USART1_SendStr(const char *str)
{
	USART_SendStr(USART1, str);
}

static inline void USART2_SendStr(const char *str)
{
	USART_SendStr(USART2, str);
}
static inline void USART3_SendStr(const char *str)
{
	USART_SendStr(USART3, str);
}

/*****************************************************************
* OOP
*****************************************************************/
void USART_AutoInit(void) __attribute__((constructor)); // thuc thi truoc khi goi ham main()

void USART_AutoInit(void)
{
	hUSART1.Init = USART1_Init;
	hUSART1.SendChar = USART1_SendChar;
	hUSART1.SendStr = USART1_SendStr;
	hUSART1.Available = USART1_Available;
	hUSART1.GetChar = USART1_GetChar;
	
	hUSART2.Init = USART2_Init;
	hUSART2.SendChar = USART2_SendChar;
	hUSART2.SendStr = USART2_SendStr;
	hUSART2.Available = USART2_Available;
	hUSART2.GetChar = USART2_GetChar;
	
	hUSART3.Init = USART3_Init;
	hUSART3.SendChar = USART3_SendChar;
	hUSART3.SendStr = USART3_SendStr;
	hUSART3.Available = USART3_Available;
	hUSART3.GetChar = USART3_GetChar;
}
__attribute__((noreturn, unused)) static void _sys_exit(int x)
{
	(void)x;
	while (1);
}
FILE __stdout;
#define DEBUG_USART   hUSART1
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
PUTCHAR_PROTOTYPE
{
	(void)f;
	if (ch == '\n')
	{
		DEBUG_USART.SendChar('\r');
	}
	DEBUG_USART.SendChar((char)ch);
	return ch;
}

#define GETCHAR_PROTOTYPE int fgetc(FILE *f)
GETCHAR_PROTOTYPE
{
	(void)f;
	while (!DEBUG_USART.Available());
	return (int)(DEBUG_USART.GetChar());
}

/*****************************************************************
* Interrupt
*****************************************************************/
void USART1_IRQHandler(void)
{
	if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{
		uint8_t c = (uint8_t)(USART_ReceiveData(USART1) & 0xFF);
		RingBuffer_PutChar(&usart1_rx, c);
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
	}
}

void USART2_IRQHandler(void)
{
	if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
	{
		uint8_t c = (uint8_t)(USART_ReceiveData(USART2) & 0xFF);
		RingBuffer_PutChar(&usart2_rx, c);
		USART_ClearITPendingBit(USART2, USART_IT_RXNE);
	}
}

void USART3_IRQHandler(void)
{
	if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
	{
		uint8_t c = (uint8_t)(USART_ReceiveData(USART3) & 0xFF);
		RingBuffer_PutChar(&usart3_rx, c);
		USART_ClearITPendingBit(USART3, USART_IT_RXNE);
	}
}
