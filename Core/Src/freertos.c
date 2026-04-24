/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "ME.h"
#include "tft_lcd.h"
#include "alarm.h"
#include "app_global.h"   
#include "app_define.h" 
#include "rtc.h"
#include "eeprom.h"
#include <string.h>
#include <stdio.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define RTC_UPDATE_INTERVAL     1000
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
uint16_t sel=0;//dispalyselect
char g_TimeStr[16];
char g_PasswordDisplay[16];
uint16_t manage=0;
static DisplayMsg_TypeDef g_LastMsg = DISPLAY_MSG_MAIN;
uint32_t lastsleepTime = 0;
/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for WarnLock */
osThreadId_t WarnLockHandle;
const osThreadAttr_t WarnLock_attributes = {
  .name = "WarnLock",
  .stack_size = 192 * 4,
  .priority = (osPriority_t) osPriorityBelowNormal1,
};
/* Definitions for key_scan */
osThreadId_t key_scanHandle;
const osThreadAttr_t key_scan_attributes = {
  .name = "key_scan",
  .stack_size = 192 * 4,
  .priority = (osPriority_t) osPriorityBelowNormal,
};
/* Definitions for Manage */
osThreadId_t ManageHandle;
const osThreadAttr_t Manage_attributes = {
  .name = "Manage",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityBelowNormal,
};
/* Definitions for Openkey */
osThreadId_t OpenkeyHandle;
const osThreadAttr_t Openkey_attributes = {
  .name = "Openkey",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityLow7,
};
/* Definitions for Show */
osThreadId_t ShowHandle;
const osThreadAttr_t Show_attributes = {
  .name = "Show",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityLow5,
};
/* Definitions for Time */
osThreadId_t TimeHandle;
const osThreadAttr_t Time_attributes = {
  .name = "Time",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for Sleep */
osThreadId_t SleepHandle;
const osThreadAttr_t Sleep_attributes = {
  .name = "Sleep",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for Initial */
osThreadId_t InitialHandle;
const osThreadAttr_t Initial_attributes = {
  .name = "Initial",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for Alarmmsg */
osMessageQueueId_t AlarmmsgHandle;
const osMessageQueueAttr_t Alarmmsg_attributes = {
  .name = "Alarmmsg"
};
/* Definitions for Settimee */
osMessageQueueId_t SettimeeHandle;
const osMessageQueueAttr_t Settimee_attributes = {
  .name = "Settimee"
};
/* Definitions for Display */
osMessageQueueId_t DisplayHandle;
const osMessageQueueAttr_t Display_attributes = {
  .name = "Display"
};
/* Definitions for sleep */
osMessageQueueId_t sleepHandle;
const osMessageQueueAttr_t sleep_attributes = {
  .name = "sleep"
};
/* Definitions for Alarm */
osTimerId_t AlarmHandle;
const osTimerAttr_t Alarm_attributes = {
  .name = "Alarm"
};
/* Definitions for Sleeptask */
osTimerId_t SleeptaskHandle;
const osTimerAttr_t Sleeptask_attributes = {
  .name = "Sleeptask"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void Alarm_PlayUnlockSuccess(void);
void Alarm_PlayWrongPassword(void);
void Alarm_PlayTimeout(void);
void Alarm_PlayAdminSuccess(void);
void Alarm_PlayLocked(void);
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void WarnLocktask(void *argument);
void key_scantask(void *argument);
void ManageTask(void *argument);
void OpenkeyTask(void *argument);
void ShowTask(void *argument);
void TimeTask(void *argument);
void SleepTask(void *argument);
void InitialTask(void *argument);
void alarmback01(void *argument);
void sleepCallback02(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* Create the timer(s) */
  /* creation of Alarm */
  AlarmHandle = osTimerNew(alarmback01, osTimerOnce, NULL, &Alarm_attributes);

  /* creation of Sleeptask */
  SleeptaskHandle = osTimerNew(sleepCallback02, osTimerPeriodic, NULL, &Sleeptask_attributes);

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* creation of Alarmmsg */
  AlarmmsgHandle = osMessageQueueNew (10, sizeof(uint16_t), &Alarmmsg_attributes);

  /* creation of Settimee */
  SettimeeHandle = osMessageQueueNew (4, sizeof(uint16_t), &Settimee_attributes);

  /* creation of Display */
  DisplayHandle = osMessageQueueNew (16, sizeof(uint16_t), &Display_attributes);

  /* creation of sleep */
  sleepHandle = osMessageQueueNew (4, sizeof(uint16_t), &sleep_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);



  /* creation of Initial */
  InitialHandle = osThreadNew(InitialTask, NULL, &Initial_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */

/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(5000);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_WarnLocktask */
/**
* @brief Function implementing the WarnLock thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_WarnLocktask */
void WarnLocktask(void *argument)
{
  /* USER CODE BEGIN WarnLocktask */
	
	AlarmMessage_TypeDef alarm_msg;
    portBASE_TYPE xStatus;
	osTimerStart(AlarmHandle,200);
  /* Infinite loop */
  for(;;)
  {
		xStatus = osMessageQueueGet(AlarmmsgHandle,&alarm_msg,NULL,portMAX_DELAY);
		 if (xStatus == osOK) {
            /* Process alarm command */
            switch (alarm_msg.command) {
                case ALARM_CMD_BEEP:
                    /* Single beep */
                    Buzzer_Beep(alarm_msg.duration);
                    break;

                case ALARM_CMD_BEEP_PATTERN:
                    /* Multiple beeps */
                    Buzzer_BeepPattern(alarm_msg.pattern);
                    break;

                case ALARM_CMD_LED_ON:
                    /* LED on */
                    LED_ON();
                    break;

                case ALARM_CMD_LED_OFF:
                    /* LED off */
                    LED_OFF();
                    break;

                case ALARM_CMD_LED_BLINK:
                    /* LED blink pattern */
                    LED_Blink(alarm_msg.duration, BEEP_INTERVAL, alarm_msg.pattern);
                    break;

                case ALARM_CMD_UNLOCK_SUCCESS:
                    /* Unlock success: 1 beep + 1 second LED */
                    Buzzer_Beep(BEEP_DURATION_LONG);
                    LED_Blink(LED_PATTERN_UNLOCK * 1000, 0, 1);
                    break;

                case ALARM_CMD_WRONG_PASSWORD:
                    /* Wrong password: 3 beeps + 2 second LED */
                    Buzzer_BeepPattern(BEEP_PATTERN_WRONG_PASSWORD);
                    LED_Blink(LED_PATTERN_WRONG_PASSWORD * 1000, BEEP_INTERVAL, 1);
                    break;

                case ALARM_CMD_TIMEOUT:
                    /* Timeout: 1 short beep */
                    Buzzer_Beep(BEEP_DURATION_SHORT);
                    break;

                case ALARM_CMD_ADMIN_SUCCESS:
                    /* Admin login success: 2 beeps */
                    Buzzer_BeepPattern(BEEP_PATTERN_ADMIN_SUCCESS);
                    break;

                case ALARM_CMD_LOCKED:
                    /* System locked: rapid LED blink for 5 seconds */
                    LED_Blink(500, 500, LED_PATTERN_LOCKED);
                    break;

                default:
                    break;
            }
        }
    osDelay(1);
  }
  /* USER CODE END WarnLocktask */
}

/* USER CODE BEGIN Header_key_scantask */
/**
* @brief Function implementing the key_scan thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_key_scantask */
void key_scantask(void *argument)
{
  /* USER CODE BEGIN key_scantask */

  /* Infinite loop */
  for(;;)
  {

    
    osDelay(1);
	}
  /* USER CODE END key_scantask */
}

/* USER CODE BEGIN Header_ManageTask */
/**
* @brief Function implementing the Manage thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_ManageTask */
void ManageTask(void *argument)
{
  /* USER CODE BEGIN ManageTask */
	uint32_t lastCheckTime = 0;
  uint32_t currentTime;
	uint16_t cont=0;
  AdminMsg_Type adminMsg;
	char managePassword[PASSWORD_LENGTH + 1];
  osStatus_t status;
  /* Infinite loop */
	
  for(;;)
  {
		currentTime = osKernelGetTickCount();

    if ((currentTime - lastCheckTime) >= 100)
    {
			 status = osMessageQueueGet(SettimeeHandle, &adminMsg, NULL, 0);
          
			switch (adminMsg.cmd)
			{
				case ADMIN_CMD_EXIT:
					if(cont!=1){
				  manage = 0;
					TFT_Clear(TFT_COLOR_WHITE);//repair
					DisplayMsg_Type msg;
          msg.msgType = DISPLAY_MSG_MAIN;
          msg.timestamp = currentTime;
          memset(msg.msgData, 0, sizeof(msg.msgData));
          if (DisplayHandle != NULL)
          {
            osMessageQueuePut(DisplayHandle, &msg, 0, 0);
          }
					cont=1;
				}
						break;
						
				case ADMIN_CMD_MODIFY_PASSWORD:
					if(cont!=2){if(manage){
					  DisplayMsg_Type msg;
            msg.msgType = DISPLAY_MSG_PASSWORD_CHANGE;
            msg.timestamp = currentTime;
            snprintf(msg.msgData, sizeof(msg.msgData), "MODIFY OK");
					if (DisplayHandle != NULL)
          {
            osMessageQueuePut(DisplayHandle, &msg, 0, 0);
          }
				   }
					cont=2;
				}
					break;
				case ADMIN_CMD_NONE:if(cont!=3){cont=3;}break;
			}
		
			if(g_InputIndex==PASSWORD_LENGTH&&manage)
					{
						
						DisplayMsg_Type msg2;
          msg2.msgType = DISPLAY_MSG_CHANGE_SUCCESS;
          msg2.timestamp = currentTime;
          memset(msg2.msgData, 0, sizeof(msg2.msgData));
          
          if (DisplayHandle != NULL)
          {
						sel=1;
            osMessageQueuePut(DisplayHandle, &msg2, 0, 0);
          }
					Alarm_PlayLocked();
					strncpy(g_Passwords, g_InputBuffer, PASSWORD_LENGTH);
					EEPROM_WriteBuffer(0, (uint8_t *)g_InputBuffer, PASSWORD_LENGTH);						
					memset(g_InputBuffer, 0, sizeof(g_InputBuffer));
          g_InputIndex = 0;
					}
		lastCheckTime = currentTime;
		}
      
    osDelay(1);
  }
  /* USER CODE END ManageTask */
}

/* USER CODE BEGIN Header_OpenkeyTask */
/**
* @brief Function implementing the Openkey thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_OpenkeyTask */
void OpenkeyTask(void *argument)
{
  /* USER CODE BEGIN OpenkeyTask */
	uint32_t lastCheckTime = 0;
  uint32_t currentTime;
  uint8_t passwordVerified = 0;
  char storedPassword[PASSWORD_LENGTH+1];
	char Password[PASSWORD_LENGTH+1]="123456";
	Password[PASSWORD_LENGTH]='\0';
	
	
  /* Infinite loop */
  for(;;)
  {
		 currentTime = osKernelGetTickCount();
    
    if ((currentTime - lastCheckTime) >= 100)
    {
      if (g_InputIndex == PASSWORD_LENGTH&&manage==0)
      {
        //EEPROM_ReadBuffer(0, (uint8_t *)storedPassword, PASSWORD_LENGTH);
        //storedPassword[PASSWORD_LENGTH] = '\0';
				//strncmp(g_Passwords,storedPassword , PASSWORD_LENGTH);
				 //g_Passwords[PASSWORD_LENGTH] = '\0';
        
        if (strncmp(g_InputBuffer,g_Passwords , PASSWORD_LENGTH) == 0)
        {
          passwordVerified = 1;
          g_ErrorCount = 0; /*  */
          
          DisplayMsg_Type msg;
          msg.msgType = DISPLAY_MSG_UNLOCK_SUCCESS;
          msg.timestamp = currentTime;
          memset(msg.msgData, 0, sizeof(msg.msgData));
          
          if (DisplayHandle != NULL)
          {
						sel=1;
            osMessageQueuePut(DisplayHandle, &msg, 0, 0);
          }
          
          osDelay(4000);
          
          HAL_GPIO_WritePin(Relay_GPIO_Port, Relay_Pin, GPIO_PIN_SET);
          
          memset(g_InputBuffer, 0, sizeof(g_InputBuffer));
          g_InputIndex = 0;
        }
        else
        {
  
          passwordVerified = 0;
          g_ErrorCount++;
          
          DisplayMsg_Type msg;
          msg.msgType = DISPLAY_MSG_PASSWORD_ERROR;
          msg.timestamp = currentTime;
          memset(msg.msgData, 0, sizeof(msg.msgData));
          
          if (DisplayHandle != NULL)
          {
						sel=1;
            osMessageQueuePut(DisplayHandle, &msg, 0, 0);
          }
          
          Alarm_PlayWrongPassword();
          
          memset(g_InputBuffer, 0, sizeof(g_InputBuffer));
          g_InputIndex = 0;
          
          if (g_ErrorCount >= MAX_ERROR_COUNT)
          {
            g_LockFlag = 1;
            
            msg.msgType = DISPLAY_MSG_SYSTEM_LOCKED;
            msg.timestamp = currentTime;
            memset(msg.msgData, 0, sizeof(msg.msgData));
            
            if (DisplayHandle != NULL)
            {
              osMessageQueuePut(DisplayHandle, &msg, 0, 0);
            }
            
            osDelay(ERROR_LOCK_TIME);
            g_LockFlag = 0;
            g_ErrorCount = 0;
          }
        }
      }
      
      lastCheckTime = currentTime;
    }
    osDelay(1);
  }
  /* USER CODE END OpenkeyTask */
}

/* USER CODE BEGIN Header_ShowTask */
/**
* @brief Function implementing the Show thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_ShowTask */
void ShowTask(void *argument)
{
  /* USER CODE BEGIN ShowTask */
	  
	  uint16_t sel1=0;
	  uint8_t lastInputIndex = 0;
	  DisplayMsg_Type msg;
    osStatus_t status;
    snprintf(g_TimeStr, sizeof(g_TimeStr), "%02d:%02d:%02d", 
             g_CurrentTime.Hours, g_CurrentTime.Minutes, g_CurrentTime.Seconds);
  /* Infinite loop */
  for(;;)
  {
		 status = osMessageQueueGet(DisplayHandle, &msg, NULL, 10);
        
        if(status == osOK) {
            
            g_LastMsg = msg.msgType;
            g_DisplayMsg = msg.msgType;
            g_DisplayUpdate = 1;
        }
        
        osMutexAcquire(g_DisplayMutexHandle, osWaitForever);
        
        if(g_SystemState == SYSTEM_STATE_SLEEP) {
            if(g_DisplayMsg != DISPLAY_MSG_SLEEP) {
                //LCD_Sleep();
            }
        } else {
            if(g_DisplayMsg == DISPLAY_MSG_SLEEP || 
               g_DisplayMsg == DISPLAY_MSG_WAKEUP) {
                //LCD_Wakeup();

            }
            
            switch(g_DisplayMsg) {
        case DISPLAY_MSG_MAIN:
					if(sel!=1)
					{LCD_ShowMainInterface();sel=1;sel1=0;}
          //LCD_ShowMainInterface();
          break;
          
        case DISPLAY_MSG_UNLOCK_SUCCESS:
					if(sel!=2)
					{LCD_ShowUnlockSuccess();sel=2;sel1=0;HAL_GPIO_WritePin(Relay_GPIO_Port, Relay_Pin, GPIO_PIN_RESET);
						Alarm_PlayUnlockSuccess();}
          //LCD_ShowUnlockSuccess();
          break;
          
        case DISPLAY_MSG_PASSWORD_ERROR:
					if(sel!=3)
					{LCD_ShowPasswordError();sel=3;sel1=0;}
          //LCD_ShowPasswordError();
          break;
          
        case DISPLAY_MSG_SYSTEM_LOCKED:
					if(sel!=4)
					{LCD_ShowSystemLocked();sel=4;sel1=0;
					Alarm_PlayLocked();}
          //LCD_ShowSystemLocked();
          break;
          
        case DISPLAY_MSG_ADMIN_MODE:
					if(sel!=5)
					{LCD_ShowAdminMode();sel=5;sel1=0;}
          //LCD_ShowAdminMode();
          break;
          
        case DISPLAY_MSG_PASSWORD_CHANGE:
					if(sel!=6)
					{LCD_ShowPasswordChangeResult(msg.msgData);sel=6;sel1=0;}
          //LCD_ShowPasswordChangeResult(msg.msgData);
          break;
          
        case DISPLAY_MSG_CHANGE_SUCCESS:
					if(sel!=7)
					{LCD_ShowChange();manage=0;sel=7;sel1=0;}
          //LCD_ShowTimeSetInterface();
          break;
          
        case DISPLAY_MSG_SLEEP:
					if(sel!=8)
					{TFT_Clear(TFT_COLOR_BLACK);sel=8;
					sel1=0;}
          //LCD_ShowSleepMode();
          break;
          
        case DISPLAY_MSG_INPUT_TIMEOUT:
					if(sel!=9)
					{LCD_ShowTimeout();Alarm_PlayTimeout();osDelay(500);TFT_Clear(TFT_COLOR_WHITE);
					osDelay(500);g_DisplayMsg = DISPLAY_MSG_MAIN;sel=9;sel1=0;}
          //LCD_ShowWakeUp();
          //osDelay(1000);
          //g_DisplayMsg = DISPLAY_MSG_MAIN;
          break;
          
        default:
					if(sel!=1)
					{LCD_ShowMainInterface();sel=1;sel1=0;}
          //LCD_ShowMainInterface();
          break;
            }
			snprintf(g_TimeStr, sizeof(g_TimeStr), "%02d:%02d:%02d", 
      g_CurrentTime.Hours, g_CurrentTime.Minutes, g_CurrentTime.Seconds);
      LCD_ShowTime(g_TimeStr);
      
      if (g_InputIndex > 0) {
				
			if(sel1!=10){TFT_Clear(TFT_COLOR_WHITE);sel1=10;}	
			
      if (g_InputIndex != lastInputIndex&&manage==0)
         {
					 
          LCD_ShowPasswordInput(g_InputBuffer, g_InputIndex);
					lastInputIndex = g_InputIndex;
         }	
      else if (g_InputIndex != lastInputIndex&&manage==1)
         {
          LCD_ShowPasswordmanage(g_InputBuffer, g_InputIndex);
					lastInputIndex = g_InputIndex;
         }				
        
        }
			}
        osMutexRelease(g_DisplayMutexHandle);
    osDelay(1);
  }
  /* USER CODE END ShowTask */
}

/* USER CODE BEGIN Header_TimeTask */
/**
* @brief Function implementing the Time thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_TimeTask */
void TimeTask(void *argument)
{
  /* USER CODE BEGIN TimeTask */
	uint32_t lastUpdateTime = 0;
  /* Infinite loop */
  for(;;)
  {
		
		uint32_t currentTime = osKernelGetTickCount();
        
        if((currentTime - lastUpdateTime) >= RTC_UPDATE_INTERVAL) {
            HAL_RTC_GetTime(&hrtc, &g_CurrentTime, RTC_FORMAT_BIN);
            HAL_RTC_GetDate(&hrtc, &g_CurrentDate, RTC_FORMAT_BIN);
            snprintf(g_TimeStr, sizeof(g_TimeStr), "%02d:%02d:%02d", 
                     g_CurrentTime.Hours, g_CurrentTime.Minutes, g_CurrentTime.Seconds);
            g_DisplayUpdate = 1;
            lastUpdateTime = currentTime;
          }
        
    osDelay(1);
  }
  /* USER CODE END TimeTask */
}

/* USER CODE BEGIN Header_SleepTask */
/**
* @brief Function implementing the Sleep thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_SleepTask */
void SleepTask(void *argument)
{
  /* USER CODE BEGIN SleepTask */
	SleepMsg_Type msg;
	uint32_t currentTime;
	SleepCmd_TypeDef g_slpmsg = SLEEP_CMD_NONE;
  /* Infinite loop */
  for(;;)
  {
		currentTime = osKernelGetTickCount();
		if ((currentTime - lastsleepTime) >= 11000)
		{
			  memset(g_InputBuffer, 0, sizeof(g_InputBuffer));
        g_InputIndex = 0;
			  //sel=0;
	      DisplayMsg_Type msg;
        msg.msgType = DISPLAY_MSG_SLEEP;
        msg.timestamp = currentTime;
        memset(msg.msgData, 0, sizeof(msg.msgData));
        
        if (DisplayHandle != NULL)
        {
	  			sel=1;
          osMessageQueuePut(DisplayHandle, &msg, 0, 0);
        }
			
		}
		else if((currentTime - lastsleepTime) >= 5000&&g_InputIndex>0)
		{
			  memset(g_InputBuffer, 0, sizeof(g_InputBuffer));
        g_InputIndex = 0;
			  //sel=0;
	      DisplayMsg_Type msg;
        msg.msgType = DISPLAY_MSG_INPUT_TIMEOUT;
        msg.timestamp = currentTime;
        memset(msg.msgData, 0, sizeof(msg.msgData));
        
        if (DisplayHandle != NULL)
        {
	  			sel=1;
          osMessageQueuePut(DisplayHandle, &msg, 0, 0);
        }
			
		}
		
      osMessageQueueGet(sleepHandle, &msg, NULL, 10);
		    g_slpmsg=msg.cmd;
        switch(g_slpmsg)
				{
					case SLEEP_CMD_NONE :break;
	        case SLEEP_CMD_START :lastsleepTime = osKernelGetTickCount();
					SleepMsg_Type slpmsg;
			    slpmsg.cmd = SLEEP_CMD_NONE;
			    if (sleepHandle != NULL)
          {
            osMessageQueuePut(sleepHandle, &slpmsg, 0, 0);
          }
	
					break;
				}

    osDelay(1);
  }
  /* USER CODE END SleepTask */
}

/* USER CODE BEGIN Header_InitialTask */
/**
* @brief Function implementing the Initial thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_InitialTask */
void InitialTask(void *argument)
{
	
	
  /* USER CODE BEGIN InitialTask */
  char defaultPassword[PASSWORD_LENGTH + 1] = "123456"; // ????
  char storedPassword[PASSWORD_LENGTH + 1];
	//defaultPassword[PASSWORD_LENGTH]='\0';
  uint8_t i;
  
  TFT_Init();
	
  MX_RTC_Init();
  
  EEPROM_Init();
	
	RTC_TimeTypeDef time;
	RTC_DateTypeDef date;
	time.Hours=20;
	time.Minutes=26;
	time.Seconds=20;
	date.Year=26;
	date.Month=4;
	date.Date=13;
	//__HAL_RTC_WRITEPROTECTION_DISABLE(&hrtc);
	//HAL_RTC_SetTime(&hrtc, &time, RTC_FORMAT_BIN);
  //HAL_RTC_SetDate(&hrtc, &date, RTC_FORMAT_BIN);
	//__HAL_RTC_WRITEPROTECTION_ENABLE(&hrtc);//Initial time;
	
  LCD_start();
	TFT_Clear(TFT_COLOR_WHITE);
	
	EEPROM_ReadBuffer(0, (uint8_t *)storedPassword, PASSWORD_LENGTH);
  storedPassword[PASSWORD_LENGTH] = '\0';
  //EEPROM_WriteBuffer(0, (uint8_t *)defaultPassword, PASSWORD_LENGTH);
  //EEPROM_ReadBuffer(0, (uint8_t *)storedPassword, PASSWORD_LENGTH);
  //storedPassword[PASSWORD_LENGTH] = '\0';
  
   
  uint16_t isEmpty = 1;
  for (i = 0; i < PASSWORD_LENGTH; i++) {
    if (storedPassword[i] != -1 && storedPassword[i] != 0x00) {
      isEmpty = 0;
      break;
    }
  }

  
 if (isEmpty) {
    EEPROM_WriteBuffer(0, (uint8_t *)defaultPassword, PASSWORD_LENGTH);
    strncpy(g_Passwords, defaultPassword, PASSWORD_LENGTH);
    g_PasswordCount = 1;
  } else {

    strncpy(g_Passwords, storedPassword, PASSWORD_LENGTH);
    g_PasswordCount = 1;
  }
  lastsleepTime = osKernelGetTickCount();

	  /* creation of WarnLock */
  WarnLockHandle = osThreadNew(WarnLocktask, NULL, &WarnLock_attributes);

  /* creation of key_scan */
  key_scanHandle = osThreadNew(key_scantask, NULL, &key_scan_attributes);

  /* creation of Manage */
  ManageHandle = osThreadNew(ManageTask, NULL, &Manage_attributes);

  /* creation of Openkey */
  OpenkeyHandle = osThreadNew(OpenkeyTask, NULL, &Openkey_attributes);

  /* creation of Show */
  ShowHandle = osThreadNew(ShowTask, NULL, &Show_attributes);

  /* creation of Time */
  TimeHandle = osThreadNew(TimeTask, NULL, &Time_attributes);

  /* creation of Sleep */
  SleepHandle = osThreadNew(SleepTask, NULL, &Sleep_attributes);

  HAL_GPIO_WritePin(Relay_GPIO_Port, Relay_Pin, GPIO_PIN_SET);
  

  g_SystemState = SYSTEM_STATE_IDLE;
  g_ErrorCount = 0;
  g_LockFlag = 0;
  g_SleepFlag = 0;
  g_AdminMode = 0;
  

  DisplayMsg_Type msg;
  msg.msgType = DISPLAY_MSG_MAIN;
  msg.timestamp = osKernelGetTickCount();
  memset(msg.msgData, 0, sizeof(msg.msgData));
  
  if (DisplayHandle != NULL) {
    osMessageQueuePut(DisplayHandle, &msg, 0, 0);
  }
  
  vTaskDelete(NULL);
  
  /* Infinite loop */

  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END InitialTask */
}

/* alarmback01 function */
void alarmback01(void *argument)
{
  /* USER CODE BEGIN alarmback01 */
HAL_GPIO_WritePin(Led_GPIO_Port, Led_Pin, GPIO_PIN_SET);
  /* USER CODE END alarmback01 */
}

/* sleepCallback02 function */
void sleepCallback02(void *argument)
{
  /* USER CODE BEGIN sleepCallback02 */
  
   memset(g_InputBuffer, 0, sizeof(g_InputBuffer));
   g_InputIndex = 0;
  /* USER CODE END sleepCallback02 */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void Alarm_SendCommand(AlarmCommand_TypeDef command, uint16_t duration, uint8_t pattern)
{
    AlarmMessage_TypeDef msg;
    msg.command = command;
    msg.duration = duration;
    msg.pattern = pattern;

    if (AlarmmsgHandle != NULL) {
        osMessageQueuePut(AlarmmsgHandle, &msg,NULL, portMAX_DELAY);
    }
}

/*===========================================
 * Helper Functions for Other Tasks
 *===========================================*/

/**
 * @brief  Play unlock success alarm
 * @retval None
 */
void Alarm_PlayUnlockSuccess(void)
{
    Alarm_SendCommand(ALARM_CMD_UNLOCK_SUCCESS, BEEP_DURATION_LONG, 1);
}

/**
 * @brief  Play wrong password alarm
 * @retval None
 */
void Alarm_PlayWrongPassword(void)
{
    Alarm_SendCommand(ALARM_CMD_WRONG_PASSWORD, BEEP_DURATION_SHORT, BEEP_PATTERN_WRONG_PASSWORD);
}

/**
 * @brief  Play timeout alarm
 * @retval None
 */
void Alarm_PlayTimeout(void)
{
    Alarm_SendCommand(ALARM_CMD_TIMEOUT, BEEP_DURATION_SHORT, 1);
}

/**
 * @brief  Play admin success alarm
 * @retval None
 */
void Alarm_PlayAdminSuccess(void)
{
    Alarm_SendCommand(ALARM_CMD_ADMIN_SUCCESS, BEEP_DURATION_SHORT, BEEP_PATTERN_ADMIN_SUCCESS);
}

/**
 * @brief  Play locked alarm
 * @retval None
 */
void Alarm_PlayLocked(void)
{
    Alarm_SendCommand(ALARM_CMD_LOCKED, 500, LED_PATTERN_LOCKED);
}

/* USER CODE END Application */

