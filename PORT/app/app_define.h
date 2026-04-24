/**
 ******************************************************************************
 * @file    app_define.h
 * @brief   应用层宏定义和枚举类型
 ******************************************************************************
 */

#ifndef __APP_DEFINE_H
#define __APP_DEFINE_H

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================
 * 系统常量定义
 * ============================================ */
#define PASSWORD_LENGTH         6           // 密码位数
#define MAX_PASSWORD_COUNT      10          // 最大密码数量
#define ERROR_LOCK_TIME         5000        // 错误锁定时间(ms)
#define INPUT_TIMEOUT           3000        // 输入超时时间(ms)
#define SLEEP_TIMEOUT           10000       // 休眠超时时间(ms)
#define MAX_ERROR_COUNT         3           // 最大错误次数
#define RTC_UPDATE_INTERVAL     1000        // RTC更新间隔(ms)
#define PASSWORD_FLASH_ADDR     0x0801F000  // 密码存储地址(最后一页)
#define ADMIN_PASSWORD          "000000"    // 管理员默认密码

/* ============================================
 * 系统状态枚举
 * ============================================ */
typedef enum {
    SYSTEM_STATE_IDLE = 0,          // 空闲状态
    SYSTEM_STATE_INPUT,             // 输入状态
    SYSTEM_STATE_VERIFY,            // 验证状态
    SYSTEM_STATE_UNLOCKED,          // 解锁状态
    SYSTEM_STATE_LOCKED,            // 锁定状态
    SYSTEM_STATE_SLEEP,             // 休眠状态
    SYSTEM_STATE_ADMIN              // 管理员模式
} SystemState_TypeDef;

/* ============================================
 * 显示消息类型枚举
 * ============================================ */
typedef enum {
    DISPLAY_MSG_MAIN = 0,           // 主界面
    DISPLAY_MSG_INPUT_TIMEOUT,      // 输入超时
    DISPLAY_MSG_UNLOCK_SUCCESS,     // 开锁成功
    DISPLAY_MSG_PASSWORD_ERROR,     // 密码错误
    DISPLAY_MSG_SYSTEM_LOCKED,       // 系统锁定
    DISPLAY_MSG_ADMIN_MODE,         // 管理员模式
    DISPLAY_MSG_PASSWORD_CHANGE,     // 修改密码
    DISPLAY_MSG_CHANGE_SUCCESS,           // 时间设置
    DISPLAY_MSG_SLEEP,              // 休眠模式
    DISPLAY_MSG_WAKEUP              // 唤醒
} DisplayMsg_TypeDef;

/* ============================================
 * 按键类型枚举
 * ============================================ */
typedef enum {
    KEY_NONE = 0,
    KEY_0, KEY_1, KEY_2, KEY_3, KEY_4,
    KEY_5, KEY_6, KEY_7, KEY_8, KEY_9,
    KEY_CLEAR,      // 清除键
    KEY_CONFIRM,    // 确认键
    KEY_ADMIN       // 管理员键(红外)
} Key_TypeDef;

/* ============================================
 * 错误类型枚举
 * ============================================ */
typedef enum {
    ERROR_NONE = 0,
    ERROR_PASSWORD_WRONG,           // 密码错误
    ERROR_INPUT_TIMEOUT,           // 输入超时
    ERROR_SYSTEM_LOCKED,           // 系统锁定
    ERROR_FLASH_WRITE,             // Flash写入错误
    ERROR_PASSWORD_EXIST,          // 密码已存在
    ERROR_PASSWORD_NOT_EXIST       // 密码不存在
} Error_TypeDef;

/* ============================================
 * 管理员命令类型枚举
 * ============================================ */
typedef enum {
    ADMIN_CMD_NONE = 0,
    //ADMIN_CMD_ADD_PASSWORD,        // 添加密码
    ADMIN_CMD_MODIFY_PASSWORD,     // 修改密码
    //ADMIN_CMD_DELETE_PASSWORD,     // 删除密码
    //ADMIN_CMD_SET_TIME,            // 设置时间
    //ADMIN_CMD_VIEW_LOG,            // 查看日志
    ADMIN_CMD_EXIT                 // 退出管理员模式
} AdminCmd_TypeDef;


typedef enum {
  SLEEP_CMD_NONE = 0,
	SLEEP_CMD_START ,
	
} SleepCmd_TypeDef;
/* ============================================
 * 消息队列定义
 * ============================================ */
#define MSG_QUEUE_SIZE          16
#define MSG_QUEUE_DISPLAY       "DisplayQueue"
#define MSG_QUEUE_PASSWORD      "PasswordQueue"
#define MSG_QUEUE_ADMIN         "AdminQueue"

/* ============================================
 * 软件定时器定义
 * ============================================ */
#define TIMER_ID_INPUT_TIMEOUT  1
#define TIMER_ID_LOCK           2
#define TIMER_ID_SLEEP          3

/* ============================================
 * 互斥锁定义
 * ============================================ */
#define MUTEX_FLASH             "FlashMutex"
#define MUTEX_DISPLAY           "DisplayMutex"
#define MUTEX_SYSTEM            "SystemMutex"

#ifdef __cplusplus
}
#endif

#endif /* __APP_DEFINE_H */
