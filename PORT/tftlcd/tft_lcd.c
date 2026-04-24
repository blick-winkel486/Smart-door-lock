#include "tft_lcd.h"
#include "main.h"
#include "FONT.H"
#include <string.h>
#include <stdio.h>
#include "app_global.h"
#include "app_define.h"
#include "FreeRTOS.h"
// 全局变量
static uint8_t tft_direction = 0;  // 显示方向
extern char g_TimeStr[16];
extern char g_PasswordDisplay[16];
#define CHAR_STAR       '*'
#define CHAR_BLANK      ' '
#define TEXT_COLOR           TFT_COLOR_BLACK
#define BG_COLOR             TFT_COLOR_WHITE
#define HIGHLIGHT_COLOR      TFT_COLOR_BLUE
#define ERROR_COLOR          TFT_COLOR_RED
#define SUCCESS_COLOR        TFT_COLOR_GREEN
// 使用FONT.H中的16x16字体 (asc2_1608)
// asc2_1608[95][16] - 95个字符，每个字符16字节，每字节8位，总共16行x16列

// 私有函数声明
static void TFT_GPIO_Init(void);
static void TFT_Delay(uint32_t delay);

// ============================================
// 底层驱动函数
// ============================================

/**
 * @brief 写命令到ILI9341
 * @param cmd: 8位命令（高8位无效）
 * @note ILI9341所有指令都是8位的
 */
void TFT_Write_Cmd(uint8_t cmd)
{
    // RS=0: 命令模式
    HAL_GPIO_WritePin(TFT_RS_GPIO_Port, TFT_RS_Pin, GPIO_PIN_RESET);
    
    // CS=0: 选中LCD
    HAL_GPIO_WritePin(TFT_CS_GPIO_Port, TFT_CS_Pin, GPIO_PIN_RESET);
    
    // WR=0: 开始写
    HAL_GPIO_WritePin(TFT_WR_GPIO_Port, TFT_WR_Pin, GPIO_PIN_RESET);
    
    // 输出8位命令到数据总线
    // 注意：ILI9341只使用低8位，高8位忽略
    // 我们需要将8位数据放到16位数据总线的低8位
    HAL_GPIO_WritePin(TFT_D0_GPIO_Port, TFT_D0_Pin, (cmd & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(TFT_D1_GPIO_Port, TFT_D1_Pin, (cmd & 0x02) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(TFT_D2_GPIO_Port, TFT_D2_Pin, (cmd & 0x04) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(TFT_D3_GPIO_Port, TFT_D3_Pin, (cmd & 0x08) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(TFT_D4_GPIO_Port, TFT_D4_Pin, (cmd & 0x10) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(TFT_D5_GPIO_Port, TFT_D5_Pin, (cmd & 0x20) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(TFT_D6_GPIO_Port, TFT_D6_Pin, (cmd & 0x40) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(TFT_D7_GPIO_Port, TFT_D7_Pin, (cmd & 0x80) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    
    // 数据总线高8位保持默认状态（ILI9341忽略高8位）
    
    // WR=1: 结束写，上升沿锁存数据
    HAL_GPIO_WritePin(TFT_WR_GPIO_Port, TFT_WR_Pin, GPIO_PIN_SET);
    
    // CS=1: 取消选中
    HAL_GPIO_WritePin(TFT_CS_GPIO_Port, TFT_CS_Pin, GPIO_PIN_SET);
}
uint8_t TFT_Read_Data(void)
{
    uint8_t data = 0;

    // RS=1: 数据模式（读ID时参数阶段必须RS=1）
    HAL_GPIO_WritePin(TFT_RS_GPIO_Port, TFT_RS_Pin, GPIO_PIN_SET);
    
    // CS=0: 选中LCD
    HAL_GPIO_WritePin(TFT_CS_GPIO_Port, TFT_CS_Pin, GPIO_PIN_RESET);
    
    // RD=0: 拉低读信号，启动读周期（8080并口标准时序）
    HAL_GPIO_WritePin(TFT_RD_GPIO_Port, TFT_RD_Pin, GPIO_PIN_RESET);
    
    // 等待数据总线稳定（根据屏幕手册时序，可根据实际调整延时）
    TFT_Delay(10);
    
    // 从D0~D7读取8位数据（高8位忽略，和写命令逻辑一致）
    if(HAL_GPIO_ReadPin(TFT_D0_GPIO_Port, TFT_D0_Pin) == GPIO_PIN_SET) data |= 0x01;
    if(HAL_GPIO_ReadPin(TFT_D1_GPIO_Port, TFT_D1_Pin) == GPIO_PIN_SET) data |= 0x02;
    if(HAL_GPIO_ReadPin(TFT_D2_GPIO_Port, TFT_D2_Pin) == GPIO_PIN_SET) data |= 0x04;
    if(HAL_GPIO_ReadPin(TFT_D3_GPIO_Port, TFT_D3_Pin) == GPIO_PIN_SET) data |= 0x08;
    if(HAL_GPIO_ReadPin(TFT_D4_GPIO_Port, TFT_D4_Pin) == GPIO_PIN_SET) data |= 0x10;
    if(HAL_GPIO_ReadPin(TFT_D5_GPIO_Port, TFT_D5_Pin) == GPIO_PIN_SET) data |= 0x20;
    if(HAL_GPIO_ReadPin(TFT_D6_GPIO_Port, TFT_D6_Pin) == GPIO_PIN_SET) data |= 0x40;
    if(HAL_GPIO_ReadPin(TFT_D7_GPIO_Port, TFT_D7_Pin) == GPIO_PIN_SET) data |= 0x80;
    
    // RD=1: 拉高读信号，结束读周期
    HAL_GPIO_WritePin(TFT_RD_GPIO_Port, TFT_RD_Pin, GPIO_PIN_SET);
    
    // CS=1: 取消选中
    HAL_GPIO_WritePin(TFT_CS_GPIO_Port, TFT_CS_Pin, GPIO_PIN_SET);

    return data;
}

/**
 * @brief 写8位数据到ILI9341
 * @param data: 8位数据
 * @note 用于写入命令参数（除GRAM外都是8位参数）
 */
void TFT_Write_Data(uint8_t data)
{
    // RS=1: 数据模式
    HAL_GPIO_WritePin(TFT_RS_GPIO_Port, TFT_RS_Pin, GPIO_PIN_SET);
    
    // CS=0: 选中LCD
    HAL_GPIO_WritePin(TFT_CS_GPIO_Port, TFT_CS_Pin, GPIO_PIN_RESET);
    
    // WR=0: 开始写
    HAL_GPIO_WritePin(TFT_WR_GPIO_Port, TFT_WR_Pin, GPIO_PIN_RESET);
    
    // 输出8位数据到数据总线低8位
    HAL_GPIO_WritePin(TFT_D0_GPIO_Port, TFT_D0_Pin, (data & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(TFT_D1_GPIO_Port, TFT_D1_Pin, (data & 0x02) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(TFT_D2_GPIO_Port, TFT_D2_Pin, (data & 0x04) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(TFT_D3_GPIO_Port, TFT_D3_Pin, (data & 0x08) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(TFT_D4_GPIO_Port, TFT_D4_Pin, (data & 0x10) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(TFT_D5_GPIO_Port, TFT_D5_Pin, (data & 0x20) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(TFT_D6_GPIO_Port, TFT_D6_Pin, (data & 0x40) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(TFT_D7_GPIO_Port, TFT_D7_Pin, (data & 0x80) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    
    // WR=1: 结束写
    HAL_GPIO_WritePin(TFT_WR_GPIO_Port, TFT_WR_Pin, GPIO_PIN_SET);
    
    // CS=1: 取消选中
    HAL_GPIO_WritePin(TFT_CS_GPIO_Port, TFT_CS_Pin, GPIO_PIN_SET);
}

/**
 * @brief 写16位数据到ILI9341
 * @param data: 16位数据（RGB565格式）
 * @note 专门用于写入GRAM数据
 */
void TFT_Write_Data16(uint16_t data)
{
    // RS=1: 数据模式
    HAL_GPIO_WritePin(TFT_RS_GPIO_Port, TFT_RS_Pin, GPIO_PIN_SET);
    
    // CS=0: 选中LCD
    HAL_GPIO_WritePin(TFT_CS_GPIO_Port, TFT_CS_Pin, GPIO_PIN_RESET);
    
    // WR=0: 开始写
    HAL_GPIO_WritePin(TFT_WR_GPIO_Port, TFT_WR_Pin, GPIO_PIN_RESET);
    
    // 输出16位数据到数据总线
    // RGB565格式：R[15:11], G[10:5], B[4:0]
    HAL_GPIO_WritePin(TFT_D0_GPIO_Port, TFT_D0_Pin, (data & 0x0001) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(TFT_D1_GPIO_Port, TFT_D1_Pin, (data & 0x0002) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(TFT_D2_GPIO_Port, TFT_D2_Pin, (data & 0x0004) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(TFT_D3_GPIO_Port, TFT_D3_Pin, (data & 0x0008) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(TFT_D4_GPIO_Port, TFT_D4_Pin, (data & 0x0010) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(TFT_D5_GPIO_Port, TFT_D5_Pin, (data & 0x0020) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(TFT_D6_GPIO_Port, TFT_D6_Pin, (data & 0x0040) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(TFT_D7_GPIO_Port, TFT_D7_Pin, (data & 0x0080) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(TFT_D8_GPIO_Port, TFT_D8_Pin, (data & 0x0100) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(TFT_D9_GPIO_Port, TFT_D9_Pin, (data & 0x0200) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(TFT_D10_GPIO_Port, TFT_D10_Pin, (data & 0x0400) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(TFT_D11_GPIO_Port, TFT_D11_Pin, (data & 0x0800) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(TFT_D12_GPIO_Port, TFT_D12_Pin, (data & 0x1000) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(TFT_D13_GPIO_Port, TFT_D13_Pin, (data & 0x2000) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(TFT_D14_GPIO_Port, TFT_D14_Pin, (data & 0x4000) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(TFT_D15_GPIO_Port, TFT_D15_Pin, (data & 0x8000) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    
    // WR=1: 结束写
    HAL_GPIO_WritePin(TFT_WR_GPIO_Port, TFT_WR_Pin, GPIO_PIN_SET);
    
    // CS=1: 取消选中
    HAL_GPIO_WritePin(TFT_CS_GPIO_Port, TFT_CS_Pin, GPIO_PIN_SET);
}

/**
 * @brief 硬件复位LCD
 */
void TFT_Reset(void)
{
    HAL_GPIO_WritePin(TFT_RST_GPIO_Port, TFT_RST_Pin, GPIO_PIN_RESET);
    HAL_Delay(100);  // 保持低电平至少10ms
    HAL_GPIO_WritePin(TFT_RST_GPIO_Port, TFT_RST_Pin, GPIO_PIN_SET);
    HAL_Delay(100);  // 等待复位完成
}

/**
 * @brief 打开背光
 * @note 需要根据实际硬件连接实现
 */
void TFT_Backlight_On(void)
{
    // 这里需要根据实际背光控制引脚实现
    // 示例：如果背光控制引脚为TFT_EN
    // HAL_GPIO_WritePin(TFT_EN_GPIO_Port, TFT_EN_GPIO_Pin, GPIO_PIN_SET);
}

/**
 * @brief 关闭背光
 */
void TFT_Backlight_Off(void)
{
    // 这里需要根据实际背光控制引脚实现
    // HAL_GPIO_WritePin(TFT_EN_GPIO_Port, TFT_EN_GPIO_Pin, GPIO_PIN_RESET);
}

// ============================================
// 私有函数
// ============================================

/**
 * @brief GPIO初始化（私有函数）
 * @note 实际上GPIO已在main.c中初始化，这里只是确保
 */
static void TFT_GPIO_Init(void)
{
    // GPIO已在MX_GPIO_Init()中初始化
    // 这里可以添加额外的GPIO配置如果需要
}

/**
 * @brief 简单延时函数
 * @param delay: 延时时间（毫秒）
 */
static void TFT_Delay(uint32_t delay)
{
    HAL_Delay(delay);
}

// ============================================
// 高级功能函数
// ============================================

/**
 * @brief 初始化TFT LCD
 */
void TFT_Init(void)
{
	
	     uint8_t lcd_id[4];
		   TFT_Read_ID(lcd_id);
    // 1. 确保GPIO已初始化
    TFT_GPIO_Init();
    
    // 2. 硬件复位
    TFT_Reset();
    
    // 3. 软件复位
    TFT_Write_Cmd(ILI9341_CMD_SWRESET);
    HAL_Delay(120);
    
    // 4. 退出睡眠模式
    TFT_Write_Cmd(ILI9341_CMD_SLEEP_OUT);
    HAL_Delay(120);
    
    // 5. 设置像素格式为16位RGB565
    TFT_Write_Cmd(ILI9341_CMD_PIXEL_FORMAT);
    TFT_Write_Data(0x55);  // 16位RGB565
    
    // 6. 设置内存访问控制
    TFT_Write_Cmd(ILI9341_CMD_MAC);
    TFT_Write_Data(0x48);  // 默认方向
    
    // 7. 设置显示方向
    TFT_Set_Direction(1);

    // 8. 打开显示
    TFT_Write_Cmd(ILI9341_CMD_DISPLAY_ON);
    HAL_Delay(20);
    
    // 9. 清屏
    TFT_Clear(TFT_COLOR_WHITE);
    
    // 10. 打开背光
    TFT_Backlight_On();
}

/**
 * @brief  读取LCD控制器ID（0xD3读ID4指令）
 * @param  id: 存储读取到的4字节ID的数组指针
 * @retval None
 */
void TFT_Read_ID(uint8_t *id)
{
    // 1. 发送读ID命令 0xD3（RS=0，对应表中"指令"行）
    TFT_Write_Cmd(0xD3);

    // 2. 读取4个参数字节（RS=1，对应表中"参数1~参数4"行）
    // 参数1:  dummy字节（表中全X，可直接读入丢弃）
	
    id[0] = TFT_Read_Data();
    // 参数2: ID1（表中0x00）
    id[1] = TFT_Read_Data();
    // 参数3: ID2（表中0x93）
    id[2] = TFT_Read_Data();
    // 参数4: ID3（表中0x41）
    id[3] = TFT_Read_Data();
}

/**
 * @brief 设置显示方向
 * @param dir: 方向 (0-3)
 */
void TFT_Set_Direction(uint8_t dir)
{
    tft_direction = dir;
    uint8_t madctl = 0x48; // 默认
    
    switch(dir)
    {
        case 0: madctl = 0x48; break; // 默认方向
        case 1: madctl = 0x28; break; // 旋转90度
        case 2: madctl = 0x88; break; // 旋转180度
        case 3: madctl = 0xE8; break; // 旋转270度
        default: break;
    }
    
    TFT_Write_Cmd(ILI9341_CMD_MAC);
    TFT_Write_Data(madctl);
}

/**
 * @brief 设置显示窗口
 * @param x1, y1: 窗口左上角坐标
 * @param x2, y2: 窗口右下角坐标
 */
void TFT_Set_Window(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    // 设置列地址
    TFT_Write_Cmd(ILI9341_CMD_COLUMN_ADDR);
TFT_Write_Data((uint8_t)(x1 >> 8));  
TFT_Write_Data((uint8_t)(x1 & 0xFF));
TFT_Write_Data((uint8_t)(x2 >> 8));     // 高8位
TFT_Write_Data((uint8_t)(x2 & 0xFF));
    
    // 设置页地址
    TFT_Write_Cmd(ILI9341_CMD_PAGE_ADDR);
TFT_Write_Data((uint8_t)(y1 >> 8));     // 高8位
TFT_Write_Data((uint8_t)(y1 & 0xFF));
TFT_Write_Data((uint8_t)(y2 >> 8));     // 高8位
TFT_Write_Data((uint8_t)(y2 & 0xFF));
    
    // 准备写GRAM
    TFT_Write_Cmd(ILI9341_CMD_GRAM_WRITE);
}

/**
 * @brief 清屏
 * @param color: RGB565颜色
 */
void TFT_Clear(uint16_t color)
{
    uint32_t i;
    TFT_Set_Window(0, 0,480, 320);
    TFT_Delay(100);
    for(i = 0; i < 153800; i++)
    {
        TFT_Write_Data16(color);
    }
}

/**
 * @brief 画点
 * @param x, y: 坐标
 * @param color: RGB565颜色
 */
void TFT_Draw_Pixel(uint16_t x, uint16_t y, uint16_t color)
{
    if(x > TFT_MAX_X || y > TFT_MAX_Y) return;
    
    TFT_Set_Window(x, y, x, y);
    TFT_Write_Data16(color);
}

/**
 * @brief 填充矩形
 * @param x, y: 左上角坐标
 * @param w, h: 宽度和高度
 * @param color: RGB565颜色
 */
void TFT_Fill_Rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
    uint16_t i, j;
    
    if(x > TFT_MAX_X || y > TFT_MAX_Y) return;
    if((x + w - 1) > TFT_MAX_X) w = TFT_MAX_X - x + 1;
    if((y + h - 1) > TFT_MAX_Y) h = TFT_MAX_Y - y + 1;
    
    TFT_Set_Window(x, y, x + w - 1, y + h - 1);
    
    for(i = 0; i < h; i++)
    {
        for(j = 0; j < w; j++)
        {
            TFT_Write_Data16(color);
        }
    }
}

/**
 * @brief 画线
 * @param x1, y1: 起点坐标
 * @param x2, y2: 终点坐标
 * @param color: RGB565颜色
 */
void TFT_Draw_Line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
    int dx = x2 - x1;
    int dy = y2 - y1;
    int step = abs(dx) > abs(dy) ? abs(dx) : abs(dy);
    float x = x1, y = y1;
    float x_inc = (float)dx / step;
    float y_inc = (float)dy / step;
    
    for(int i = 0; i <= step; i++)
    {
        TFT_Draw_Pixel((uint16_t)x, (uint16_t)y, color);
        x += x_inc;
        y += y_inc;
    }
}

/**
 * @brief 显示一个字符
 * @param x, y: 显示位置
 * @param ch: 字符
 * @param color: 字符颜色
 * @param bg_color: 背景颜色
 * @param size: 字体大小（1:8x16, 2:16x32等）
 * @note 使用FONT.H中的asc2_1608字体（8x16），支持简单缩放
 */
void TFT_Show_Char(uint16_t x, uint16_t y, uint8_t ch, uint16_t color, uint16_t bg_color, uint8_t size)
{
    uint8_t i, j, k, l;
    uint8_t temp, chr;
    uint16_t x0 = x;
    
    if(ch < 32 || ch > 127) return; // 只显示可打印ASCII字符
    if(size == 0) size = 1; // 最小尺寸为1
    
    chr = ch - 32; // 字符索引（ASCII从32开始）
    
    // 使用asc2_0808字体（8x8）
    for(i = 0; i < 8; i++) // 8行
    {
        temp = asc2_0808[chr][i]; // 获取当前行的像素数据
        
        // 绘制当前行的8个像素，支持缩放
        for(j = 0; j < 8; j++) // 8列
        {
            uint8_t pixel = (temp & 0x80) ? 1 : 0; // 获取像素值
            temp <<= 1; // 左移一位，检查下一个像素
            
            // 根据size缩放像素
            for(k = 0; k < size; k++) // 垂直缩放
            {
                for(l = 0; l < size; l++) // 水平缩放
                {
                    if(pixel)
                        TFT_Draw_Pixel(x + l, y + k, color);
                    else
                        TFT_Draw_Pixel(x + l, y + k, bg_color);
                }
            }
            x += size; // 移动到下一个像素列（考虑缩放）
        }
        x = x0; // 回到行首
        y += size; // 移动到下一行（考虑缩放）
    }
}


static void LCD_ShowCenteredText(uint16_t y, const char* text, uint8_t size)
{
    uint16_t textWidth = strlen(text) * 8 * size;  // 估算文本宽度
    uint16_t x;
    
    // 防止溢出，如果文本宽度大于屏幕宽度，则从左边开始显示
    if (textWidth > TFT_WIDTH) {
        x = 0;
    } else {
        x = (TFT_WIDTH - textWidth) / 2;
    }
    
    TFT_Show_Str(x, y, (char*)text, TFT_COLOR_BLACK, TFT_COLOR_WHITE, size);
}


static void LCD_ShowHeader(const char* title)
{
    // 显示标题
    LCD_ShowCenteredText(20, title, 2);
    
    // 画一条分隔线
    TFT_Draw_Line(20, 50, TFT_WIDTH - 20, 50, TFT_COLOR_BLACK);
}

/**
 * @brief 显示字符串
 * @param x, y: 显示位置
 * @param str: 字符串
 * @param color: 字符颜色
 * @param bg_color: 背景颜色
 * @param size: 字体大小
 */

void TFT_Show_Str(uint16_t x, uint16_t y, char *str, uint16_t color, uint16_t bg_color, uint8_t size)
{
    while(*str)
    {
        TFT_Show_Char(x, y, *str, color, bg_color, size);
        x += 8 * size;
        str++;
        
        if(x > TFT_MAX_X - 8 * size)
        {
            x = 0;
            y += 16 * size;
        }
    }
}



void LCD_ShowMainInterface(void)
{
    
    // 显示标题
    LCD_ShowHeader("DOOCR LOCK SYSTEM");
    
    // 显示提示信息
    LCD_ShowCenteredText(80, "ENTER PASSWORD", 2);
    LCD_ShowCenteredText(120, "******", 3);
    
    // 显示底部提示
    TFT_Show_Str(100, 200, "PRESS * TO CONFIRM", TEXT_COLOR, BG_COLOR, 1);
    TFT_Show_Str(120, 220, "PRESS # TO CLEAR", TEXT_COLOR, BG_COLOR, 1);
	
	  //TFT_Show_Str(40, 280, g_TimeStr, TEXT_COLOR, BG_COLOR, 2);
}

void LCD_ShowPasswordInput(const char* input, uint8_t len)
{
    char displayStr[7] = {0};
    
    // 创建显示字符串（用*表示已输入的字符）
    for (uint8_t i = 0; i < PASSWORD_LENGTH; i++) {
        if (i < len) {
            displayStr[i] = '*';
        } else {
            displayStr[i] = '_';
        }
    }
    displayStr[PASSWORD_LENGTH] = '\0';
    
    //TFT_Clear(BG_COLOR);
    LCD_ShowHeader("ENTER PASSWORD");
    
    // 显示密码输入框
    TFT_Show_Str(160,120,displayStr, HIGHLIGHT_COLOR, BG_COLOR, 3);
    
    // 显示输入提示
    char infoStr[32];
    snprintf(infoStr, sizeof(infoStr), "INPUT: %d/%d", len, PASSWORD_LENGTH);
    TFT_Show_Str(160, 150, infoStr, TEXT_COLOR, BG_COLOR, 1);
}

void LCD_ShowPasswordmanage(const char* input, uint8_t len)
{
    char displayStr[7] = {0};
    
    // 创建显示字符串（用*表示已输入的字符）
    for (uint8_t i = 0; i < PASSWORD_LENGTH; i++) {
        if (i < len) {
            displayStr[i] = '*';
        } else {
            displayStr[i] = '_';
        }
    }
    displayStr[PASSWORD_LENGTH] = '\0';
    
    //TFT_Clear(BG_COLOR);
    LCD_ShowHeader("CHANGE PASSWORD");
    
    // 显示密码输入框
    TFT_Show_Str(160,120,displayStr, HIGHLIGHT_COLOR, BG_COLOR, 3);
    
    // 显示输入提示
    char infoStr[32];
    snprintf(infoStr, sizeof(infoStr), "INPUT: %d/%d", len, PASSWORD_LENGTH);
    TFT_Show_Str(160, 150, infoStr, TEXT_COLOR, BG_COLOR, 1);
}

void LCD_ShowUnlockSuccess(void)
{
    TFT_Clear(BG_COLOR);
    LCD_ShowHeader("SUCCESS");
    
    LCD_ShowCenteredText(100, "DOOR UNLOCKED", 3);
    LCD_ShowCenteredText(140, "SUCCESSFULLY", 3);
    
    TFT_Show_Str(150, 200, "PLEASE ENTER", SUCCESS_COLOR, BG_COLOR, 2);
    TFT_Show_Str(170, 230, "WITHIN 3 SECONDS", SUCCESS_COLOR, BG_COLOR, 2);
}

void LCD_ShowPasswordError(void)
{
    TFT_Clear(BG_COLOR);
    LCD_ShowHeader("ERROR");
    
    LCD_ShowCenteredText(100, "WRONG PASSWORD", 3);
    LCD_ShowCenteredText(140, "PLEASE TRY AGAIN", 2);
    
    TFT_Show_Str(150, 200, "ERROR COUNT:", ERROR_COLOR, BG_COLOR, 1);
    
    char errorStr[16];
    snprintf(errorStr, sizeof(errorStr), "%d/%d", g_ErrorCount, MAX_ERROR_COUNT);
    TFT_Show_Str(280, 200, errorStr, ERROR_COLOR, BG_COLOR, 1);
}


void LCD_ShowTimeout(void)
{
    TFT_Clear(BG_COLOR);
    LCD_ShowHeader("ERROR");
    
    LCD_ShowCenteredText(100, "Timeout", 3);
}

void LCD_ShowChange(void)
{
    TFT_Clear(BG_COLOR);
    LCD_ShowHeader("CHANGEC SUCCESS");
    
    
    LCD_ShowCenteredText(140, "RETURN MENU", 3);
    
}


/**
 * @brief  Display system locked message
 * @retval None
 */
void LCD_ShowSystemLocked(void)
{
    TFT_Clear(BG_COLOR);
    LCD_ShowHeader("SYSTEM LOCKED");
    
    LCD_ShowCenteredText(100, "TOO MANY ERRORS", 3);
    LCD_ShowCenteredText(140, "SYSTEM LOCKED", 3);
    
    TFT_Show_Str(150, 180, "PLEASE WAIT", ERROR_COLOR, BG_COLOR, 2);
    
    char timeStr[32];
    snprintf(timeStr, sizeof(timeStr), "%d SECONDS", ERROR_LOCK_TIME / 1000);
    TFT_Show_Str(180, 210, timeStr, ERROR_COLOR, BG_COLOR, 2);
}

/**
 * @brief  Display admin mode interface
 * @retval None
 */
void LCD_ShowAdminMode(void)
{
    TFT_Clear(BG_COLOR);
    LCD_ShowHeader("ADMIN MODE");
    
    TFT_Show_Str(80, 80, "1. ADD PASSWORD", TEXT_COLOR, BG_COLOR, 2);
    TFT_Show_Str(80, 110, "2. MODIFY PASSWORD", TEXT_COLOR, BG_COLOR, 2);
    TFT_Show_Str(80, 140, "3. DELETE PASSWORD", TEXT_COLOR, BG_COLOR, 2);
    TFT_Show_Str(80, 170, "4. SET TIME", TEXT_COLOR, BG_COLOR, 2);
    TFT_Show_Str(80, 200, "5. VIEW LOG", TEXT_COLOR, BG_COLOR, 2);
    TFT_Show_Str(80, 230, "0. EXIT", TEXT_COLOR, BG_COLOR, 2);
    
    TFT_Show_Str(150, 260, "SELECT OPTION:", HIGHLIGHT_COLOR, BG_COLOR, 1);
}

/**
 * @brief  Display password change result
 * @param  result: Result message (e.g., "ADD OK", "EXIST", "FULL", etc.)
 * @retval None
 */

void LCD_ShowPasswordChangeResult(const char* result)
{
    TFT_Clear(BG_COLOR);
    LCD_ShowHeader("PASSWORD CHANGE");
    
    
    // 根据结果类型选择颜色
    LCD_ShowCenteredText(100, "ADMIN MODE", 3);
    
}

/**
 * @brief  Display time set interface
 * @retval None
 */
void LCD_ShowTimeSetInterface(void)
{
    TFT_Clear(BG_COLOR);
    LCD_ShowHeader("SET TIME");
    
    TFT_Show_Str(100, 100, "CURRENT TIME:", TEXT_COLOR, BG_COLOR, 2);
    
    char timeStr[16];
    snprintf(timeStr, sizeof(timeStr), "%02d:%02d:%02d", 
             g_CurrentTime.Hours, g_CurrentTime.Minutes, g_CurrentTime.Seconds);
    TFT_Show_Str(180, 130, timeStr, HIGHLIGHT_COLOR, BG_COLOR, 3);
    
    TFT_Show_Str(80, 180, "FORMAT: HHMMSS", TEXT_COLOR, BG_COLOR, 1);
    TFT_Show_Str(80, 200, "EXAMPLE: 143000", TEXT_COLOR, BG_COLOR, 1);
    TFT_Show_Str(80, 220, "FOR 14:30:00", TEXT_COLOR, BG_COLOR, 1);
}

/**
 * @brief  Display sleep mode
 * @retval None
 */
void LCD_ShowSleepMode(void)
{
    TFT_Clear(BG_COLOR);
    LCD_ShowHeader("SYSTEM SLEEP");
    
    LCD_ShowCenteredText(120, "SYSTEM IN", 2);
    LCD_ShowCenteredText(160, "SLEEP MODE", 2);
    
    TFT_Show_Str(150, 220, "PRESS ANY KEY", TEXT_COLOR, BG_COLOR, 1);
    TFT_Show_Str(170, 240, "TO WAKE UP", TEXT_COLOR, BG_COLOR, 1);
}

/**
 * @brief  Display wake up message
 * @retval None
 */
void LCD_ShowWakeUp(void)
{
    TFT_Clear(BG_COLOR);
    LCD_ShowHeader("WAKE UP");
    
    LCD_ShowCenteredText(120, "SYSTEM", 3);
    LCD_ShowCenteredText(160, "WAKING UP", 3);
    
    // 短暂显示后自动切换到主界面
}

void LCD_ClearScreen(void)
{
    TFT_Clear(BG_COLOR);
}

/**
 * @brief  Display current time
 * @param  timeStr: Time string in format "HH:MM:SS"
 * @retval None
 */

void LCD_ShowTime(const char* timeStr)
{
    // 在屏幕右上角显示时间
    TFT_Show_Str(350, 10, (char*)timeStr, TEXT_COLOR, BG_COLOR, 1);
}

void LCD_start(void)
{
	  uint16_t pos=40;//80
	  uint16_t neg=0;
    TFT_Clear(BG_COLOR);
    while(neg<=120)
    {
			LCD_ShowCenteredText(pos-neg+20,"                 ", 3);
			LCD_ShowCenteredText(pos+40-neg+20,"               ", 3);
			LCD_ShowCenteredText(pos-neg,"SMART DOOR LOCK", 3);
	    LCD_ShowCenteredText(pos+40-neg,"HuZhetao 2080", 3);
			neg=neg+20;
		}
    
}

