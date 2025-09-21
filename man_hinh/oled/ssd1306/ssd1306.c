#include "ssd1306.h"
#include <string.h>
#include <math.h>
#include <stdlib.h>

/*********************************************************************
 * PRIVATE VARIABLE
 **********************************************************************/
static I2C_TypeDef *ssd1306_I2Cx;
static uint8_t SSD1306_Buffer[MAX_COL * MAX_PAGE]; // 128x64 / 8 = 1024 bytes
static ssd1306_Handle_t SSD1306;

/*********************************************************************
 * FUNCTION
 **********************************************************************/
void ssd1306_SendCommand(uint8_t cmd)
{
	uint8_t buffer[2];
	buffer[0] = 0x00;
	buffer[1] = cmd;
	i2c_Master_Transmit(ssd1306_I2Cx, SSD1306_ADDR, buffer, sizeof(buffer));
}

void ssd1306_SendData(uint8_t cmd)
{
	uint8_t buffer[2];
	buffer[0] = 0x40;
	buffer[1] = cmd;
	i2c_Master_Transmit(ssd1306_I2Cx, SSD1306_ADDR, buffer, sizeof(buffer));
}

void ssd1306_Init(I2C_TypeDef *I2Cx)
{
	ssd1306_I2Cx = I2Cx;

	i2c_Init(ssd1306_I2Cx);

	ssd1306_SendCommand(0xAE); // Display OFF
	ssd1306_SendCommand(0x20); // Set Memory Addressing Mode
	ssd1306_SendCommand(0x10); // Horizontal addressing mode
	ssd1306_SendCommand(0xB0); // Set Page Start Address
	ssd1306_SendCommand(0xC8); // COM Output Scan Direction remapped
	ssd1306_SendCommand(0x00); // Set low column address
	ssd1306_SendCommand(0x10); // Set high column address
	ssd1306_SendCommand(0x40); // Set start line address

	ssd1306_SendCommand(0x81);
	ssd1306_SendCommand(0x7F); // Contrast

	ssd1306_SendCommand(0xA1); // Segment re-map
	ssd1306_SendCommand(0xA6); // Normal display

	ssd1306_SendCommand(0xA8);
	ssd1306_SendCommand(0x3F); // Multiplex ratio 1/64

	ssd1306_SendCommand(0xA4); // Output follows RAM content

	ssd1306_SendCommand(0xD3);
	ssd1306_SendCommand(0x00); // Display offset

	ssd1306_SendCommand(0xD5);
	ssd1306_SendCommand(0x80); // Display clock

	ssd1306_SendCommand(0xD9);
	ssd1306_SendCommand(0xF1); // Pre-charge period

	ssd1306_SendCommand(0xDA);
	ssd1306_SendCommand(0x12); // COM pins

	ssd1306_SendCommand(0xDB);
	ssd1306_SendCommand(0x40); // VCOM detect

	ssd1306_SendCommand(0x8D);
	ssd1306_SendCommand(0x14); // Enable charge pump

	ssd1306_SendCommand(0xAF); // Display ON

	ssd1306_Clear();
	ssd1306_Refresh();
}

void ssd1306_Clear(void)
{
	memset(SSD1306_Buffer, 0x00, sizeof(SSD1306_Buffer));
}

void ssd1306_DisplayOn(void)
{
	ssd1306_SendCommand(0xAF);
}
void ssd1306_DisplayOff(void)
{
	ssd1306_SendCommand(0xAE);
}
void ssd1306_Refresh(void)
{
	for (uint8_t page = 0; page < MAX_PAGE; page++)
	{
		ssd1306_SendCommand(0xB0 + page);
		ssd1306_SendCommand(0x00);
		ssd1306_SendCommand(0x10);

		uint8_t buffer[129];
		buffer[0] = 0x40;
		for (uint8_t col = 0; col < MAX_COL; col++)
		{
			buffer[1 + col] = SSD1306_Buffer[page * MAX_COL + col];
		}
		i2c_Master_Transmit(ssd1306_I2Cx, SSD1306_ADDR, buffer, sizeof(buffer));
	}
}

void ssd1306_SetContrast(uint8_t value)
{
	ssd1306_SendCommand(0x81);
	ssd1306_SendCommand(value);
}

void ssd1306_DrawPixel(uint8_t x, uint8_t y, ssd1306_Color_t color)
{
	if (x >= MAX_COL || y >= MAX_ROW)
	{
		return;
	}

	if (color == COLOR_WHITE)
	{
		SSD1306_Buffer[x + (y / 8) * MAX_COL] |= (1 << (y % 8));
	}
	else
	{
		SSD1306_Buffer[x + (y / 8) * MAX_COL] &= ~(1 << (y % 8));
	}
}

void ssd1306_SetCursor(uint8_t x, uint8_t y)
{
	SSD1306.currentX = x;
	SSD1306.currentY = y;
}

uint8_t ssd1306_PutChar(char ch, FontTypedef_t Font, ssd1306_Color_t color)
{
	uint8_t i, j;
	uint32_t block;
	for (i = 0; i < Font.height; i++)
	{
		block = Font.data[(ch - 32) * Font.height + i];
		for (j = 0; j < Font.width; j++)
		{
			if ((block << j) & 0x8000)
			{
				ssd1306_DrawPixel(SSD1306.currentX + j, SSD1306.currentY + i, color);
			}
			else
			{
				ssd1306_DrawPixel(SSD1306.currentX + j, SSD1306.currentY + i, !color);
			}
		}
	}
	SSD1306.currentX += Font.width;
	return ch;
}

uint8_t ssd1306_PutString(char *str, FontTypedef_t Font, ssd1306_Color_t color)
{
	while (*str)
	{
		if (ssd1306_PutChar(*str, Font, color) != *str)
		{
			return *str;
		}
		str++;
	}
	return 0;
}

void ssd1306_FillBuffer(uint8_t data)
{
	memset(SSD1306_Buffer, data, sizeof(SSD1306_Buffer));
}

void ssd1306_SetPixel(uint8_t x, uint8_t y)
{
	ssd1306_DrawPixel(x, y, COLOR_WHITE);
}

void ssd1306_SetAutoNewline(FunctionalState state)
{
	SSD1306.autoNewLine = state;
}

void ssd1306_DrawLine(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, ssd1306_Color_t color)
{
	int32_t deltaX = abs(x2 - x1);
	int32_t deltaY = abs(y2 - y1);
	int32_t signX = ((x1 < x2) ? 1 : -1);
	int32_t signY = ((y1 < y2) ? 1 : -1);
	int32_t error = deltaX - deltaY;
	int32_t error2;

	ssd1306_DrawPixel(x2, y2, color);

	while ((x1 != x2) || (y1 != y2))
	{
		ssd1306_DrawPixel(x1, y1, color);
		error2 = error * 2;
		if (error2 > -deltaY)
		{
			error -= deltaY;
			x1 += signX;
		}

		if (error2 < deltaX)
		{
			error += deltaX;
			y1 += signY;
		}
	}
	return;
}
void ssd1306_FillRectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, ssd1306_Color_t color)
{
	uint8_t x, y;
	uint8_t x_start = ((x1 <= x2) ? x1 : x2);
	uint8_t x_end = ((x1 <= x2) ? x2 : x1);
	uint8_t y_start = ((y1 <= y2) ? y1 : y2);
	uint8_t y_end = ((y1 <= y2) ? y2 : y1);

	for (y = y_start; (y <= y_end) && (y < MAX_ROW); y++)
	{
		for (x = x_start; (x <= x_end) && (x < MAX_COL); x++)
		{
			ssd1306_DrawPixel(x, y, color);
		}
	}
}
void ssd1306_DrawImage(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const uint8_t *bitmap)
{
	if (x >= MAX_COL || y >= MAX_ROW)
	{
		return;
	}

	uint16_t byteWidth = (w + 7) / 8;
	for (uint8_t j = 0; j < h; j++)
	{
		for (uint8_t i = 0; i < w; i++)
		{
			uint8_t byte = bitmap[j * byteWidth + i / 8];
			if (byte & (0x80 >> (i % 8)))
			{
				ssd1306_DrawPixel(x + i, y + j, COLOR_WHITE);
			}
			else
			{
				ssd1306_DrawPixel(x + i, y + j, COLOR_BLACK);
			}
		}
	}
}

void ssd1306_DrawBitMap(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const uint8_t *bitmap)
{
	if (x >= MAX_COL || y >= MAX_ROW)
	{
		return;
	}

	uint16_t byteWidth = w;
	uint16_t byteHeight = (h + 7) / 8;

	for (uint16_t j = 0; j < byteHeight; j++)
	{
		for (uint16_t i = 0; i < w; i++)
		{
			uint8_t byte = bitmap[j * byteWidth + i];
			for (uint8_t bit = 0; bit < 8; bit++)
			{
				uint16_t py = y + j * 8 + bit;
				if (py >= MAX_ROW)
					break;

				if (byte & (1 << bit))
				{
					ssd1306_DrawPixel(x + i, py, COLOR_WHITE);
				}
				else
				{
					ssd1306_DrawPixel(x + i, py, COLOR_BLACK);
				}
			}
		}
	}
}
