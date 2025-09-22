#include "st7735.h"
#include "delay.h"

/*
 * @brief gui lenh command den LCD
 * @param Cmd: lenh command
 * @retval None
 */
void st7735_SendCmd(uint8_t Cmd)
{
	ST7735_CS_LOW();
	GPIO_ResetBits(DC_Port, DC_Pin);
	spi_Master_Transmit(Cmd);
	ST7735_CS_HIGH();
}

/*
 * @brief gui lenh data den LCD
 * @param Cmd: lenh data
 * @retval None
 */
void st7735_SendData(uint8_t Data)
{
	ST7735_CS_LOW();
	GPIO_SetBits(DC_Port, DC_Pin);
	spi_Master_Transmit(Data);
	ST7735_CS_HIGH();
}

/*
 * @brief gui danh sach lenh command den LCD
 * @param CmdList: con tro danh sach lenh command
 * @retval None
 */
void st7735_SendCmdList(const uint8_t *CmdList)
{
	uint8_t dat = 0;
	uint8_t cmd = 0;
	uint8_t num = 0;

	while (1)
	{
		cmd = *CmdList++;
		num = *CmdList++;
		if (cmd == LCD_CMD_DELAY_MS)
		{
			if (num == LCD_CMD_EOF)
			{
				break;
			}
			else
			{
				DelayMs((uint32_t)(num * 10));
			}
		}
		else
		{
			st7735_SendCmd(cmd);
			for (dat = 0; dat < num; dat++)
			{
				st7735_SendData(*CmdList++);
			}
		}
	}
}

/*
 * @brief dat khung hinh ve LCD
 * @param x_start: toa do x bat dau
 * @param y_start: toa do y bat dau
 * @param x_end: toa do x ket thuc
 * @param y_end: toa do y ket thuc
 * @retval None
 */
void st7735_SetWindow(uint8_t x_start, uint8_t y_start, uint8_t x_end, uint8_t y_end)
{
	st7735_SendCmd(ST7735_CASET);
	st7735_SendData(x_start >> 8);
	st7735_SendData(x_start & 0xFF);
	st7735_SendData(x_end >> 8);
	st7735_SendData(x_end & 0xFF);

	st7735_SendCmd(ST7735_RASET);
	st7735_SendData(y_start >> 8);
	st7735_SendData(y_start & 0xFF);
	st7735_SendData(y_end >> 8);
	st7735_SendData(y_end & 0xFF);

	st7735_SendCmd(ST7735_RAMWR);
}

/*
 * @brief to toan bo man hinh voi mot mau co dinh
 * Ham nay se:
 * - Gui lenh Memory Wirte (0x2C) de bat dau ghi du dieu pixel
 * - Gui du lieu mau cho toan bo 128*160 pixel (RGB565, big-endian)
 * @param color: mau can to
 * @retval None
 */
void st7735_FillScreen(uint16_t color)
{
	st7735_SendCmd(ST7735_RAMWR);
	int i;
	for (i = 0; i < 128 * 160; i++)
	{
		st7735_SendData(color >> 8);
		st7735_SendData(color & 0xFF);
	}
}

void st7735_FillScreen_Fast(uint16_t color)
{
	st7735_SetWindow(0, 0, (uint8_t)(ST7735_WIDTH - 1), (uint8_t)(ST7735_HEIGHT - 1));

	uint8_t hi = color >> 8;
	uint8_t lo = color & 0xFF;

	ST7735_CS_LOW();
	GPIO_SetBits(DC_Port, DC_Pin);
	for (int i = 0; i < ST7735_WIDTH * ST7735_HEIGHT; i++)
	{
		spi_Master_Transmit(hi);
		spi_Master_Transmit(lo);
	}
	ST7735_CS_HIGH();
}

void st7735_DrawPixel(uint8_t x, uint8_t y, uint16_t color)
{
	if (x >= ST7735_WIDTH || y >= ST7735_HEIGHT)
	{
		return;
	}
	st7735_SetWindow(x, y, x + 1, y + 1);
	st7735_SendCmd(ST7735_RAMWR);
	st7735_SendData(color >> 8);
	st7735_SendData(color & 0xFF);
}

void st7735_PutChar(uint8_t x, uint8_t y, uint8_t kitu, Font_Define_t f, uint16_t color, uint16_t background)
{
	uint8_t i, j;
	uint16_t pixelData;
	for (i = 0; i < f.height; i++)
	{
		pixelData = f.data[(kitu - 32) * f.height + i];
		for (j = 0; j < f.width; j++)
		{
			if ((pixelData << j) & 0x8000)
			{
				st7735_DrawPixel(x + j, y + i, color);
			}
			else
			{
				st7735_DrawPixel(x + j, y + i, background);
			}
		}
	}
}

void st7735_PutString(uint8_t x, uint8_t y, const char *str, Font_Define_t f, uint16_t color, uint16_t background)
{
	while (*str)
	{
		st7735_PutChar(x, y, *str, f, color, background);
		x += f.width;
		str++;
	}
}

void st7735_FillRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint16_t color)
{
	if ((x >= ST7735_WIDTH) || (y >= ST7735_HEIGHT))
		return;

	if ((x + w - 1) >= ST7735_WIDTH)
	{
		w = (uint8_t)ST7735_WIDTH - x;
	}

	if ((y + h - 1) >= ST7735_HEIGHT)
	{
		h = (uint8_t)ST7735_HEIGHT - y;
	}

	st7735_SetWindow(x, y, x + w - 1, y + h - 1);
	st7735_SendCmd(ST7735_RAMWR);

	ST7735_CS_LOW();
	GPIO_SetBits(DC_Port, DC_Pin);
	for (uint32_t i = 0; i < (uint32_t)w * h; i++)
	{
		spi_Master_Transmit(color >> 8);
		spi_Master_Transmit(color & 0xFF);
	}
	ST7735_CS_HIGH();
}

void st7735_FillCircle(uint8_t x0, uint8_t y0, uint8_t r, uint16_t color)
{
	int16_t x = 0;
	int16_t y = r;
	int16_t d = 3 - 2 * r;

	while (y >= x)
	{
		st7735_FillRect((uint8_t)(x0 - x), (uint8_t)(y0 - y), (uint8_t)(2 * x + 1), (uint8_t)(2 * y + 1), color);
		st7735_FillRect((uint8_t)(x0 - y), (uint8_t)(y0 - x), (uint8_t)(2 * y + 1), (uint8_t)(2 * x + 1), color);

		if (d < 0)
		{
			d = d + 4 * x + 6;
		}
		else
		{
			d = d + 4 * (x - y) + 10;
			y--;
		}
		x++;
	}
}

void st7735_InvertColors(_Bool Invert)
{
	ST7735_CS_LOW();
	st7735_SendCmd(Invert ? ST7735_INVON : ST7735_INVOFF);
	ST7735_CS_HIGH();
}

void st7735_BitMap(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const uint16_t *data)
{
	uint32_t i;

	if ((x >= ST7735_WIDTH) || (y >= ST7735_HEIGHT))
		return;
	if ((x + w - 1) >= ST7735_WIDTH)
		w = (uint8_t)ST7735_WIDTH - x;
	if ((y + h - 1) >= ST7735_HEIGHT)
		h = (uint8_t)ST7735_HEIGHT - y;

	st7735_SetWindow(x, y, x + w - 1, y + h - 1);

	ST7735_DC_HIGH();
	ST7735_CS_LOW();

	for (i = 0; i < (uint32_t)w * h; i++)
	{
		spi_Master_Transmit(data[i] >> 8);
		spi_Master_Transmit(data[i] & 0xFF);
	}
	ST7735_CS_HIGH();
}

void st7735_Direction(uint8_t direction)
{
	st7735_SendCmd(ST7735_MADCTL);
	switch (direction)
	{
	case 0:
		st7735_SendData(0x00);
		ST7735_WIDTH = 128;
		ST7735_HEIGHT = 160;
		break;
	case 1:
		st7735_SendData(0x60);
		ST7735_WIDTH = 160;
		ST7735_HEIGHT = 128;
		break;
	case 2:
		st7735_SendData(0xC0);
		ST7735_WIDTH = 128;
		ST7735_HEIGHT = 160;
		break;
	case 3:
		st7735_SendData(0xA0);
		ST7735_WIDTH = 160;
		ST7735_HEIGHT = 128;
		break;
	default:
		break;
	}
}

/*
 * @brief khoi tao man hinh LCD ST7735
 * Ham nay thuc hien qua trinh khoi tao phan cung va cau hinh co ban
 * cho man hinh ST7735, bao gom:
 * - Cau hinh cac chan GPIO (DC, CS, RST)
 * - Reset phan cung man hinh bang cach keo RST xuong roi keo len
 * - Gui cac lenh khoi tao co ban (Software Reset, Sleep Out)
 * - Gui danh sach lenh cau hinh tu u8PanelCmdList
 * - Cau hinh huong pixel (MADCTL)
 * - Cau hinh dinh dang mau (COLMOD)
 * - Tat che do dao mau (INVOFF)
 * - Dat cua so hien thi (Set Window)
 * - Bat hien thi (Display ON)
 * @retval None
 */
void st7735_Init(void)
{
	GPIOx_Init(DC_Port, DC_Pin, GPIO_Mode_Out_PP, GPIO_Speed_50MHz);
	GPIOx_Init(CS_Port, CS_Pin, GPIO_Mode_Out_PP, GPIO_Speed_50MHz);
	GPIOx_Init(RST_PORT, RST_PIN, GPIO_Mode_Out_PP, GPIO_Speed_50MHz);
	GPIO_ResetBits(RST_PORT, RST_PIN);
	DelayMs(20);
	GPIO_SetBits(RST_PORT, RST_PIN);
	DelayMs(105);
	st7735_SendCmd(ST7735_SWRESET);
	DelayMs(105);
	st7735_SendCmd(ST7735_SLPOUT);
	DelayMs(174);

	st7735_SendCmdList(u8PanelCmdList);

	st7735_SendCmd(ST7735_MADCTL);
	st7735_SendData(0x08);

	st7735_SendCmd(ST7735_COLMOD);
	st7735_SendData(0x05);

	st7735_SendCmd(ST7735_INVOFF);

	st7735_SetWindow(0, 0, 128, 160);

	st7735_SendCmd(ST7735_DISPON);
	DelayMs(105);
}
