#include "st7735.h"
#include "gpio.h"
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
	uint8_t index = 0;
	uint8_t cmd = 0;
	uint8_t num = 0;

	while (1)
	{
		cmd = *CmdList++;
		num = *CmdList++;
		if (cmd == LCD_CMD_EOF)
		{
			break;
		}
		else
		{
			st7735_SendCmd(cmd);
			for (index = 0; index < num; index++)
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
	st7735_SendCmd(0x2A);
	st7735_SendData(0x00);
	st7735_SendData(x_start);
	st7735_SendData(0x00);
	st7735_SendData(x_end - 1);

	st7735_SendCmd(0x2B);
	st7735_SendData(0x00);
	st7735_SendData(y_start);
	st7735_SendData(0x00);
	st7735_SendData(y_end - 2);
}

/*
 * @brief to toan bo man hinh voi mot mau co dinh
 * Ham nay se:
 * - Gui lenh Memory Wirte (0x2C) de bat dau ghi du dieu pixel
 * - Gui du lieu mau cho toan bo 128*160 pixel (RGB565, big-endian)
 * @param color: mau can to
 * @retval None
 */
void st7735_FillDisplay(uint16_t color)
{
	st7735_SendCmd(0x2C);
	int i;
	for (i = 0; i < 128 * 160; i++)
	{
		st7735_SendData(color >> 8);
		st7735_SendData(color & 0xFF);
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
