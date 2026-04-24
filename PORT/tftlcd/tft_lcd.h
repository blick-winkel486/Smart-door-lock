#ifndef TFT_LCD_H
#define TFT_LCD_H

#include "main.h"
#include "stm32f1xx_hal.h"

// ILI9341命令定义
#define ILI9341_CMD_SWRESET         0x01    // 软件复位
#define ILI9341_CMD_SLEEP_OUT       0x11    // 睡眠模式关闭
#define ILI9341_CMD_GAMMA_SET       0x26    // Gamma设置
#define ILI9341_CMD_DISPLAY_OFF     0x28    // 显示关闭
#define ILI9341_CMD_DISPLAY_ON      0x29    // 显示开启
#define ILI9341_CMD_COLUMN_ADDR     0x2A    // 列地址设置
#define ILI9341_CMD_PAGE_ADDR       0x2B    // 页地址设置
#define ILI9341_CMD_GRAM_WRITE      0x2C    // 写GRAM
#define ILI9341_CMD_GRAM_READ       0x2E    // 读GRAM
#define ILI9341_CMD_MAC             0x36    // 内存访问控制
#define ILI9341_CMD_PIXEL_FORMAT    0x3A    // 像素格式
#define ILI9341_CMD_FRAME_CTRL      0xB1    // 帧率控制
#define ILI9341_CMD_DISPLAY_CTRL    0xB6    // 显示控制
#define ILI9341_CMD_POWER_CTRL1     0xC0    // 电源控制1
#define ILI9341_CMD_POWER_CTRL2     0xC1    // 电源控制2
#define ILI9341_CMD_VCOM_CTRL1      0xC5    // VCOM控制1
#define ILI9341_CMD_VCOM_CTRL2      0xC7    // VCOM控制2
#define ILI9341_CMD_POWER_CTRLA     0xCB    // 电源控制A
#define ILI9341_CMD_POWER_CTRLB     0xCF    // 电源控制B
#define ILI9341_CMD_DRIVER_TIMINGA  0xE8    // 驱动时序控制A
#define ILI9341_CMD_DRIVER_TIMINGB  0xEA    // 驱动时序控制B
#define ILI9341_CMD_POWER_ON_SEQ    0xED    // 上电序列控制
#define ILI9341_CMD_ENABLE_3G       0xF2    // 3G控制
#define ILI9341_CMD_PUMP_RATIO      0xF7    // 泵比率控制
#define ILI9341_CMD_PGAMMA          0xE0    // 正极性Gamma校正
#define ILI9341_CMD_NGAMMA          0xE1    // 负极性Gamma校正

// 颜色定义 (RGB565格式)
#define TFT_COLOR_BLACK         0x0000
#define TFT_COLOR_WHITE         0xFFFF
#define TFT_COLOR_RED           0xF800
#define TFT_COLOR_GREEN         0x07E0
#define TFT_COLOR_BLUE          0x001F
#define TFT_COLOR_YELLOW        0xFFE0
#define TFT_COLOR_MAGENTA       0xF81F
#define TFT_COLOR_CYAN          0x07FF


#define lcd1 1
// LCD尺寸定义
#if lcd1
#define TFT_WIDTH               480
#define TFT_HEIGHT              320
#else
#define TFT_WIDTH               300
#define TFT_HEIGHT              200
#endif
#define TFT_MAX_X               (TFT_WIDTH - 1)
#define TFT_MAX_Y               (TFT_HEIGHT - 1)




// 函数声明
void TFT_Write_Cmd(uint8_t cmd);
void TFT_Write_Data(uint8_t data);
void TFT_Write_Data16(uint16_t data);
void TFT_Reset(void);
void TFT_Backlight_On(void);
void TFT_Backlight_Off(void);
void TFT_Init(void);
void TFT_Set_Direction(uint8_t dir);
void TFT_Set_Window(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void TFT_Clear(uint16_t color);
void TFT_Draw_Pixel(uint16_t x, uint16_t y, uint16_t color);
void TFT_Fill_Rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
void TFT_Draw_Line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
void TFT_Show_Char(uint16_t x, uint16_t y, uint8_t ch, uint16_t color, uint16_t bg_color, uint8_t size);
void TFT_Show_Str(uint16_t x, uint16_t y, char *str, uint16_t color, uint16_t bg_color, uint8_t size);
void TFT_Read_ID(uint8_t *id);
void LCD_ShowMainInterface(void);
void LCD_ShowUnlockSuccess(void);
void LCD_ShowTime(const char* timeStr);
void LCD_ClearScreen(void);
void LCD_ShowWakeUp(void);
void LCD_ShowSleepMode(void);
void LCD_ShowTimeSetInterface(void);
void LCD_ShowPasswordChangeResult(const char* result);
void LCD_ShowAdminMode(void);
void LCD_ShowSystemLocked(void);
void LCD_ShowPasswordError(void);
void LCD_ShowPasswordInput(const char* input, uint8_t len);
void LCD_ShowPasswordmanage(const char* input, uint8_t len);
void LCD_ShowChange(void);
void LCD_ShowTimeout(void);
void LCD_start(void);
#endif // TFT_LCD_H