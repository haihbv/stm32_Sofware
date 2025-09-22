#ifndef ST7735_H
#define ST7735_H

#ifdef __cplusplus
extern "C"
{
#endif

/*
* @file    st7735.h
* @brief   This is TFT LCD ST7735 driver 
* @author  haihbv
* @date    09/2025
*/

#include "spi.h"
#include "gpio.h"
#include "fonts.h"

/*
   ST7735   ->  STM32F103 (SPI1)
   ----------------------------
   VCC      ->  3.3V        // Nguon cap
   GND      ->  GND         // Mass
   SCL/CLK  ->  PA5 (SCK)   // Clock SPI
   SDA/MOSI ->  PA7 (MOSI)  // Du lieu tu MCU gui den LCD
   RES/RST  ->  PA2 (GPIO)  // Reset phan cung LCD
   DC       ->  PA3 (GPIO)  // Data/Command select
   CS       ->  PA4 (GPIO)  // Chip Select (NSS)
   BL       ->  3.3V / PA1  // Den nen
*/

static int16_t ST7735_WIDTH  = 128;
static int16_t ST7735_HEIGHT = 160;

/*********************************************************************
 * Defines
 **********************************************************************/
#define DC_Port             GPIOA
#define DC_Pin              GPIO_Pin_3

#define CS_Port             GPIOA
#define CS_Pin              GPIO_Pin_4

#define RST_PORT            GPIOA
#define RST_PIN             GPIO_Pin_2

#define ST7735_CS_LOW()     GPIO_ResetBits(CS_Port, CS_Pin)
#define ST7735_CS_HIGH()    GPIO_SetBits(CS_Port, CS_Pin)
#define ST7735_DC_LOW()     GPIO_ResetBits(DC_Port, DC_Pin)
#define ST7735_DC_HIGH()    GPIO_SetBits(DC_Port, DC_Pin)

#define LCD_CMD_EOF         0xFF
#define LCD_CMD_DELAY_MS    0xFF


/*********************************************************************
 * ST7735 Command List
 **********************************************************************/
#define ST7735_NOP       0x00  // No operation 
#define ST7735_SWRESET   0x01  // Software reset 
#define ST7735_SLPIN     0x10  // Sleep in 
#define ST7735_SLPOUT    0x11  // Sleep out 
#define ST7735_PTLON     0x12  // Partial mode on 
#define ST7735_NORON     0x13  // Normal display mode on 
#define ST7735_INVOFF    0x20  // Display inversion off 
#define ST7735_INVON     0x21  // Display inversion on 
#define ST7735_DISPOFF   0x28  // Display off 
#define ST7735_DISPON    0x29  // Display on 
#define ST7735_CASET     0x2A  // Column address set 
#define ST7735_RASET     0x2B  // Row address set 
#define ST7735_RAMWR     0x2C  // Memory write 
#define ST7735_RAMRD     0x2E  // Memory read 
#define ST7735_MADCTL    0x36  // Memory data access control 
#define ST7735_COLMOD    0x3A  // Interface pixel format 

static const uint8_t u8PanelCmdList[] = {
    //  Command     Length      Data
    0xB1,           0x03,       0x01, 0x2C, 0x2D,                                                                               // Frame Rate Control (In normal mode/ Full colors)
    0xB2,           0x03,       0x01, 0x2C, 0x2D,                                                                               // Frame Rate Control (In Idle mode/ 8-colors)
    0xB3,           0x06,       0x01, 0x2C, 0x2D, 0x01, 0x2C, 0x2D,                                                             // Frame Rate Control (In Partial mode/ full colors)
    0xB4,           0x01,       0x07,                                                                                           // Display Inversion Control
    0xC0,           0x03,       0xA2, 0x02, 0x84,                                                                               // Power Control 1
    0xC1,           0x01,       0xC5,                                                                                           // Power Control 2
    0xC2,           0x02,       0x0A, 0x00,                                                                                     // Power Control 3 (in Normal mode/ Full colors)
    0xC3,           0x02,       0x8A, 0x2A,                                                                                     // Power Control 4 (in Idle mode/ 8-colors)
    0xC4,           0x02,       0x8A, 0xEE,                                                                                     // Power Control 5 (in Partial mode/ full-colors)
    0xC5,           0x01,       0x0E,                                                                                           // VCOM Control 1
    0xE0,           0x10,       0x02, 0x1C, 0x07, 0x12, 0x37, 0x32, 0x29, 0x2D, 0x29, 0x25, 0x2B, 0x39, 0x00, 0x01, 0x03, 0x10, // Gamma adjustment(+ polarity)
    0xE1,           0x10,       0x03, 0x1D, 0x07, 0x06, 0x2E, 0x2C, 0x29, 0x2D, 0x2E, 0x2E, 0x37, 0x3F, 0x00, 0x00, 0x02, 0x10, // Gamma adjustment(- polarity)
    LCD_CMD_DELAY_MS, LCD_CMD_EOF};

/*********************************************************************
 * RGB Color
 **********************************************************************/
#define RGB_565(red, green, blue) (((red & 0xF8) << 8) | ((green & 0xFC) << 3) | ((blue & 0xF8) >> 3))
#define BLACK     0x0000  
#define WHITE     0xFFFF  
#define BLUE      0xF800  
#define GREEN     0x07E0  
#define RED       0x001F  
#define YELLOW    0x07FF  
#define CYAN    	0xFFE0  
#define MAGENTA   0xF81F  
#define PINK      0xFC9F  
#define PURPLE    0x780F  
#define GRAY      0x8410  

/*********************************************************************
 * API Prototype
 **********************************************************************/
void st7735_SendCmd(uint8_t Cmd);
void st7735_SendData(uint8_t Data);
void st7735_SendCmdList(const uint8_t *CmdList);
void st7735_SetWindow(uint8_t x_start, uint8_t y_start, uint8_t x_end, uint8_t y_end);
void st7735_FillScreen(uint16_t color);
void st7735_FillScreen_Fast(uint16_t color);
void st7735_DrawPixel(uint8_t x, uint8_t y, uint16_t color);
void st7735_PutChar(uint8_t x, uint8_t y, uint8_t kitu, Font_Define_t f, uint16_t color, uint16_t background);
void st7735_PutString(uint8_t x, uint8_t y, const char *str, Font_Define_t f, uint16_t color, uint16_t background);
void st7735_FillRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint16_t color);
void st7735_FillCircle(uint8_t x0, uint8_t y0, uint8_t r, uint16_t color);
void st7735_InvertColors(_Bool Invert);
void st7735_Direction(uint8_t direction);
void st7735_BitMap(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const uint16_t *data);
void st7735_Init(void);

#ifdef __cplusplus
}
#endif /* extern "C" */

#endif /* ST7735_H */
