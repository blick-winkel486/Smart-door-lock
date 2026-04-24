/**
 ******************************************************************************
 * @file    app_global.c
 * @brief   全局变量定义
 ******************************************************************************
 */

#include "app_global.h"
#include "app_define.h"
#include "cmsis_os.h"

/* ============================================
 * 系统状态相关
 * ============================================ */
volatile SystemState_TypeDef g_SystemState = SYSTEM_STATE_IDLE;
volatile uint8_t g_ErrorCount = 0;
volatile uint8_t g_LockFlag = 0;
volatile uint8_t g_SleepFlag = 0;
volatile uint8_t g_AdminMode = 0;
volatile uint32_t g_LastActivityTime = 0;


/* ============================================
 * 密码相关
 * ============================================ */
char g_InputBuffer[PASSWORD_LENGTH + 1] = {0};
uint8_t g_InputIndex = 0;
char g_Passwords[PASSWORD_LENGTH + 1];//char g_Passwords[MAX_PASSWORD_COUNT][PASSWORD_LENGTH + 1];
uint8_t g_PasswordCount = 0;

/* ============================================
 * 时间相关
 * ============================================ */
RTC_TimeTypeDef g_CurrentTime = {0};
RTC_DateTypeDef g_CurrentDate = {0};

/* ============================================
 * 显示相关
 * ============================================ */
volatile uint8_t g_DisplayUpdate = 1;
DisplayMsg_TypeDef g_DisplayMsg = DISPLAY_MSG_MAIN;

/* ============================================
 * 红外遥控相关
 * ============================================ */
volatile uint32_t g_IRCode = 0;
volatile uint8_t g_IRReady = 0;

/* ============================================
 * 互斥锁和信号量定义
 * ============================================ */
osMutexId_t g_FlashMutexHandle = NULL;
osMutexId_t g_DisplayMutexHandle = NULL;
osMutexId_t g_SystemMutexHandle = NULL;

osMessageQueueId_t g_DisplayQueueHandle = NULL;//DisplayHandle
osMessageQueueId_t g_PasswordQueueHandle = NULL;
osMessageQueueId_t g_AdminQueueHandle = NULL;

osTimerId_t g_InputTimerHandle = NULL;
osTimerId_t g_LockTimerHandle = NULL;
osTimerId_t g_SleepTimerHandle = NULL;

/* ============================================
 * 任务句柄定义
 * ============================================ */
osThreadId_t g_InitTaskHandle = NULL;
osThreadId_t g_DisplayTaskHandle = NULL;
osThreadId_t g_KeypadTaskHandle = NULL;
osThreadId_t g_PasswordTaskHandle = NULL;
osThreadId_t g_AlarmTaskHandle = NULL;
osThreadId_t g_IRAdminTaskHandle = NULL;
osThreadId_t g_RTCTaskHandle = NULL;
osThreadId_t g_SleepTaskHandle = NULL;
