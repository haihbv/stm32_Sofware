#ifndef __OLED__
#define __OLED__

#include "stm32f10x.h"
#include "i2c.h"
#include "fonts.h"

/*********************************************************************
 * DEFINE
 **********************************************************************/

#define SSD1306_ADDR 0x78
#define MAX_PAGE 0x08
#define MAX_COL 0x80
#define MAX_ROW 0x40

/*********************************************************************
 * TYPEDEF
 **********************************************************************/
typedef enum
{
	COLOR_BLACK = 0x00,
	COLOR_WHITE = 0xFF
} ssd1306_Color_t;

typedef struct
{
	uint8_t currentX;
	uint8_t currentY;
	FunctionalState autoNewLine;
} ssd1306_Handle_t;

/*********************************************************************
 * API Prototype
 **********************************************************************/
void ssd1306_SendCommand(uint8_t cmd);
void ssd1306_SendData(uint8_t data);
void ssd1306_Init(I2C_TypeDef *I2Cx);
void ssd1306_Clear(void);
void ssd1306_DisplayOn(void);
void ssd1306_DisplayOff(void);
void ssd1306_Refresh(void);
void ssd1306_SetContrast(uint8_t value);
void ssd1306_DrawPixel(uint8_t x, uint8_t y, ssd1306_Color_t color);
void ssd1306_SetCursor(uint8_t x, uint8_t y);

void ssd1306_FillBuffer(uint8_t data);
void ssd1306_SetPixel(uint8_t x, uint8_t y);
void ssd1306_SetAutoNewline(FunctionalState state);

uint8_t ssd1306_PutChar(char ch, FontTypedef_t Font, ssd1306_Color_t color);
uint8_t ssd1306_PutString(char *str, FontTypedef_t Font, ssd1306_Color_t color);

void ssd1306_DrawLine(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, ssd1306_Color_t color);
void ssd1306_FillRectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, ssd1306_Color_t color);
void ssd1306_DrawImage(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const uint8_t *bitmap);
void ssd1306_DrawBitMap(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const uint8_t *bitmap);

#endif /* __OLED__ */
