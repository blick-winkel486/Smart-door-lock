#include "cmsis_os.h"
#include "main.h"
#include "alarm.h"


extern 	osTimerId_t AlarmHandle;

void BUZZER_ON(void)
{
HAL_GPIO_WritePin(Beep_GPIO_Port, Beep_Pin, GPIO_PIN_RESET);
}

void BUZZER_OFF(void)
{
HAL_GPIO_WritePin(Beep_GPIO_Port, Beep_Pin, GPIO_PIN_SET);
}

void LED_ON(void)
{HAL_GPIO_WritePin(Led_GPIO_Port, Led_Pin, GPIO_PIN_RESET);
	osTimerStart(AlarmHandle,2000);
}

void LED_OFF(void)
{HAL_GPIO_WritePin(Led_GPIO_Port, Led_Pin, GPIO_PIN_SET);
}

void Buzzer_Beep(uint16_t duration_ms)
{
    BUZZER_ON();
    HAL_Delay(duration_ms);
    BUZZER_OFF();
}

/**
 * @brief  Sound buzzer beep pattern
 * @param  pattern: Number of beeps
 * @retval None
 */
void Buzzer_BeepPattern(uint8_t pattern)
{
    for (uint8_t i = 0; i < pattern; i++) {
        BUZZER_ON();
        HAL_Delay(BEEP_DURATION_SHORT);
        BUZZER_OFF();

        if (i < pattern - 1) {
            HAL_Delay(BEEP_INTERVAL);
        }
    }
}

/**
 * @brief  LED blink pattern
 * @param  on_time: LED on time in milliseconds
 * @param  off_time: LED off time in milliseconds
 * @param  count: Number of blinks
 * @retval None
 */
void LED_Blink(uint16_t on_time, uint16_t off_time, uint8_t count)
{
    for (uint8_t i = 0; i < count; i++) {
        LED_ON();
        HAL_Delay(on_time);

        if (off_time > 0) {
            LED_OFF();
            HAL_Delay(off_time);
        }
    }

    /* Ensure LED is off at the end */
    LED_OFF();
}

/**
 * @brief  LED set state
 * @param  state: 0 = off, 1 = on
 * @retval None
 */
void LED_SetState(uint8_t state)
{
    if (state) {
        LED_ON();
    } else {
        LED_OFF();
    }
}

/**
 * @brief  Send alarm command
 * @param  command: Alarm command
 * @param  duration: Duration in ms
 * @param  pattern: Number of repetitions
 * @retval None
 */


