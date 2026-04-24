#include "ME.h"
#include "main.h"
#include "gpio.h"
#include "tft_lcd.h"
#include "app_global.h"
#include "app_define.h"
#include "freertos.h"
#include <string.h>

extern osTimerId_t SleeptaskHandle;
extern uint16_t manage;
extern osMessageQueueId_t SettimeeHandle;
extern osMessageQueueId_t sleepHandle;

const uint8_t key_map[4][4] = {
    {'1','2','3','A'},
    {'4','5','6','B'},
    {'7','8','9','C'},
    {'*','0','#','D'}
};


void key1_scan()
{

   uint8_t num='0';
	for(int r=0;r<4;r++)
{
	  HAL_GPIO_WritePin(GPIOC, line1_Pin|line2_Pin|line3_Pin|line4_Pin, GPIO_PIN_SET);
	switch(r)
	{
		case(0):
			HAL_GPIO_WritePin(GPIOC, line1_Pin, GPIO_PIN_RESET);
		break;
		case(1):
			HAL_GPIO_WritePin(GPIOC, line2_Pin, GPIO_PIN_RESET);
		break;
		case(2):
			HAL_GPIO_WritePin(GPIOC,line3_Pin, GPIO_PIN_RESET);
		break;
		case(3):
			HAL_GPIO_WritePin(GPIOC,line4_Pin, GPIO_PIN_RESET);
		break;
	}
	osDelay(5);
	for(int l=0;l<4;l++)
	{
		uint16_t row_pin = 0;
		switch(l)
		{
			case(0):
				row_pin=row1_Pin;
				break;
			case(1):
				row_pin=row2_Pin;
				break;
			case(2):
				row_pin=row3_Pin;
				break;
			case(3): 
        row_pin=row4_Pin;
				break;

		}
			     if(HAL_GPIO_ReadPin(GPIOC,row_pin)==GPIO_PIN_RESET)			
		 {  
			 SleepMsg_Type slpmsg;
			 slpmsg.cmd = SLEEP_CMD_START;
			 if (sleepHandle != NULL)
          {
            osMessageQueuePut(sleepHandle, &slpmsg, 0, 0);
          }
			 while(HAL_GPIO_ReadPin(GPIOC,row_pin) == GPIO_PIN_RESET);
			 osDelay(5);
			 num=key_map[r][l];
			 if(num=='*'){
		     if(g_InputIndex>0)
				 {
					 g_InputBuffer[g_InputIndex]=NULL;g_InputIndex--;
				   if(g_InputIndex==0)
					 {
						 LCD_ShowPasswordInput(g_InputBuffer, g_InputIndex);
						 
					 }
				 }
				 break;
			 }
			 else if(num=='A'){
				 	manage=1;//intial keyword,cont=1
				  memset(g_InputBuffer, 0, sizeof(g_InputBuffer));
          g_InputIndex = 0;
		      AdminMsg_Type msg;
          msg.cmd = ADMIN_CMD_MODIFY_PASSWORD;
          //msg.timestamp = currentTime;
          memset(msg.data, 0, sizeof(msg.data));
				 if (SettimeeHandle != NULL)
          {
            osMessageQueuePut(SettimeeHandle, &msg, 0, 0);
          }
						break;
				 break;
			 }
			 else if(num=='B'){
				  memset(g_InputBuffer, 0, sizeof(g_InputBuffer));
          g_InputIndex = 0;
		      AdminMsg_Type msg;
          msg.cmd = ADMIN_CMD_EXIT;
          //msg.timestamp = currentTime;
          memset(msg.data, 0, sizeof(msg.data));
				 if (SettimeeHandle != NULL)
          {
            osMessageQueuePut(SettimeeHandle, &msg, 0, 0);
          }
				 break;
			 }
			 else if(num=='d'){
		     //TFT_Show_Char(210, 160, '1',TFT_COLOR_BLACK, TFT_COLOR_WHITE, 3);
				 //TFT_Show_Char(234, 160, '3',TFT_COLOR_BLACK, TFT_COLOR_WHITE, 3);
				 break;
			 }
			 else if(num=='e'){
		     //TFT_Show_Char(210, 160, '1',TFT_COLOR_BLACK, TFT_COLOR_WHITE, 3);
				 //TFT_Show_Char(234, 160, '4',TFT_COLOR_BLACK, TFT_COLOR_WHITE, 3);
				 break;
			 }
			 else if(num=='f'){
		     //TFT_Show_Char(210, 160, '1',TFT_COLOR_BLACK, TFT_COLOR_WHITE, 3);
				 //TFT_Show_Char(234, 160, '5',TFT_COLOR_BLACK, TFT_COLOR_WHITE, 3);
				 break;
			 }
			 else if(num=='g'){
		     //TFT_Show_Char(210, 160, '1',TFT_COLOR_BLACK, TFT_COLOR_WHITE, 3);
				 //TFT_Show_Char(234, 160, '6',TFT_COLOR_BLACK, TFT_COLOR_WHITE, 3);
				 break;
			 }
			 else 
			 {
				
				 if(g_InputIndex<PASSWORD_LENGTH)
				 {g_InputBuffer[g_InputIndex]=num;g_InputIndex++;}
				 else if (g_InputIndex==PASSWORD_LENGTH)
				 {g_InputBuffer[g_InputIndex]='\0';}
			 }
        
			 }
		 }
	}
		HAL_GPIO_WritePin(GPIOC, line1_Pin|line2_Pin|line3_Pin|line4_Pin, GPIO_PIN_RESET);
}

