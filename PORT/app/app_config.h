/**
 ******************************************************************************
 * @file    app_config.h
 * @brief   应用配置文件
 ******************************************************************************
 */

#ifndef __APP_CONFIG_H
#define __APP_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================
 * 系统配置
 * ============================================ */
#define SYSTEM_NAME             "Smart Door Lock"
#define SYSTEM_VERSION          "V1.0.2026"
#define HARDWARE_VERSION        "HW:STM32F103C8"

/* ============================================
 * 功能开关
 * ============================================ */
#define ENABLE_PASSWORD_LOG     1           // 启用密码日志
#define ENABLE_ADMIN_LOG        1           // 启用管理员日志
#define ENABLE_RTC              1           // 启用RTC
#define ENABLE_IR_RECEIVER      1           // 启用红外遥控
#define ENABLE_LCD_BACKLIGHT    0           // 启用LCD背光控制

/* ============================================
 * 调试配置
 * ============================================ */
#define DEBUG_MODE              0           // 调试模式
#define DEBUG_UART              0           // UART调试输出

#if DEBUG_MODE
    #define DEBUG_PRINTF(...)    printf(__VA_ARGS__)
#else
    #define DEBUG_PRINTF(...)
#endif

/* ============================================
 * 硬件引脚配置
 * ============================================ */
#define LCD_DATA_PORT           GPIOB
#define LCD_CTRL_PORT           GPIOB

#define KEYPAD_COL_PORT         GPIOA
#define KEYPAD_ROW_PORT         GPIOA

#define BEEP_PORT               GPIOC
#define RELAY_PORT              GPIOC
#define LED_PORT                GPIOC
#define IR_PORT                 GPIOA

/* ============================================
 * 时序参数
 * ============================================ */
#define DELAY_SHORT             10          // 短延时(ms)
#define DELAY_MEDIUM            100         // 中延时(ms)
#define DELAY_LONG              500         // 长延时(ms)
#define DELAY_XLONG             1000        // 超长延时(ms)

/* ============================================
 * 默认设置
 * ============================================ */
#define DEFAULT_UNLOCK_TIME     1000        // 默认开锁时间(ms)
#define DEFAULT_PASSWORD         "123456"
#define DEFAULT_ADMIN_PASSWORD   "000000"

#ifdef __cplusplus
}
#endif

#endif /* __APP_CONFIG_H */
