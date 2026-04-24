/**
 ******************************************************************************
 * @file    app_global.h
 * @brief   全局变量和结构体定义
 ******************************************************************************
 */

#ifndef __APP_GLOBAL_H
#define __APP_GLOBAL_H

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================
 * 头文件包含
 * ============================================ */
#include "app_define.h"
#include "cmsis_os.h"
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_rtc.h"

/* ============================================
 * 全局变量声明(需要互斥保护)
 * ============================================ */

/* 系统状态相关 */
extern volatile SystemState_TypeDef g_SystemState;      // 系统状态
extern volatile uint8_t g_ErrorCount;                   // 错误计数
extern volatile uint8_t g_LockFlag;                     // 锁定标志
extern volatile uint8_t g_SleepFlag;                    // 休眠标志
extern volatile uint8_t g_AdminMode;                    // 管理员模式标志
extern volatile uint32_t g_LastActivityTime;            // 最后操作时间戳


/* 密码相关 */
extern char g_InputBuffer[PASSWORD_LENGTH + 1];         // 输入缓冲区
extern uint8_t g_InputIndex;                            // 输入索引
extern char g_Passwords[PASSWORD_LENGTH + 1]; // 密码数组extern char g_Passwords[MAX_PASSWORD_COUNT][PASSWORD_LENGTH + 1];
extern uint8_t g_PasswordCount;                         // 密码数量

/* 时间相关 */
extern RTC_TimeTypeDef g_CurrentTime;                   // 当前时间
extern RTC_DateTypeDef g_CurrentDate;                  // 当前日期

/* 显示相关 */
extern volatile uint8_t g_DisplayUpdate;               // 显示更新标志
extern DisplayMsg_TypeDef g_DisplayMsg;                // 显示消息

/* 红外遥控相关 */
extern volatile uint32_t g_IRCode;                     // 红外码值
extern volatile uint8_t g_IRReady;                      // 红外就绪标志

/* ============================================
 * 消息结构体定义
 * ============================================ */

/**
 * @brief 显示消息结构体
 */
typedef struct {
    DisplayMsg_TypeDef msgType;         // 消息类型
    char msgData[32];                    // 消息数据
    uint32_t timestamp;                 // 时间戳
} DisplayMsg_Type;

/**
 * @brief 密码消息结构体
 */
typedef struct {
    char password[PASSWORD_LENGTH + 1];  // 密码
    uint8_t isAdmin;                     // 是否管理员密码
    uint32_t timestamp;                  // 时间戳
} PasswordMsg_Type;

/**
 * @brief 管理员命令结构体
 */
typedef struct {
    AdminCmd_TypeDef cmd;                // 命令类型
    char data[64];                        // 命令数据
    //uint32_t timestamp;                  // 时间戳
} AdminMsg_Type;

/**
 * @brief 按键事件结构体
 */
typedef struct {
    Key_TypeDef key;                     // 按键类型
    uint32_t timestamp;                  // 按下时间戳
} KeyEvent_Type;


typedef struct {
    SleepCmd_TypeDef cmd;                // 命令类型
} SleepMsg_Type;
/* ============================================
 * 互斥锁和信号量声明
 * ============================================ */
extern osMutexId_t g_FlashMutexHandle;          // Flash互斥锁
extern osMutexId_t g_DisplayMutexHandle;        // 显示互斥锁
extern osMutexId_t g_SystemMutexHandle;        // 系统互斥锁

extern osMessageQueueId_t DisplayHandle; // 显示队列
extern osMessageQueueId_t g_PasswordQueueHandle;// 密码队列
extern osMessageQueueId_t g_AdminQueueHandle;  // 管理员队列

extern osTimerId_t g_InputTimerHandle;          // 输入超时定时器
extern osTimerId_t g_LockTimerHandle;          // 锁定定时器
extern osTimerId_t g_SleepTimerHandle;          // 休眠定时器

/* ============================================
 * 任务句柄声明
 * ============================================ */
extern osThreadId_t g_InitTaskHandle;           // 初始化任务
extern osThreadId_t g_DisplayTaskHandle;        // 显示任务
extern osThreadId_t g_KeypadTaskHandle;         // 键盘任务
extern osThreadId_t g_PasswordTaskHandle;       // 密码任务
extern osThreadId_t g_AlarmTaskHandle;          // 报警任务
extern osThreadId_t g_IRAdminTaskHandle;        // 红外管理员任务
extern osThreadId_t g_RTCTaskHandle;            // RTC任务
extern osThreadId_t g_SleepTaskHandle;          // 休眠任务

#ifdef __cplusplus
}
#endif

#endif /* __APP_GLOBAL_H */
