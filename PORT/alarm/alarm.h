#ifndef ALARM_H
#define ALARM_H

#include "cmsis_os.h"
#include "main.h"


#define BEEP_PATTERN_TIMEOUT                 1       /* 1 beep for timeout */
#define BEEP_PATTERN_WRONG_PASSWORD          3       /* 3 beeps for wrong password */
#define BEEP_PATTERN_UNLOCK_SUCCESS          1       /* 1 long beep for unlock */
#define BEEP_PATTERN_ADMIN_SUCCESS           2       /* 2 beeps for admin login */
#define BEEP_PATTERN_ERROR                   5       /* 5 rapid beeps for error */

#define BEEP_DURATION_SHORT                  100     /* 100ms short beep */
#define BEEP_DURATION_LONG                   300     /* 300ms long beep */
#define BEEP_INTERVAL                        150     /* 150ms between beeps */

/*===========================================
 * LED Patterns
 *===========================================*/
#define LED_PATTERN_UNLOCK                   1       /* 1 second on */
#define LED_PATTERN_WRONG_PASSWORD           2       /* 2 seconds blink */
#define LED_PATTERN_LOCKED                   5       /* 5 seconds blink */
#define LED_PATTERN_ADMIN                   1       /* 1 second blink */

/*===========================================
 * Queue Message Definitions
 *===========================================*/
typedef enum {
    ALARM_CMD_NONE = 0,
    ALARM_CMD_BEEP,
    ALARM_CMD_LED_ON,
    ALARM_CMD_LED_OFF,
    ALARM_CMD_LED_BLINK,
    ALARM_CMD_BEEP_PATTERN,
    ALARM_CMD_UNLOCK_SUCCESS,
    ALARM_CMD_WRONG_PASSWORD,
    ALARM_CMD_TIMEOUT,
    ALARM_CMD_ADMIN_SUCCESS,
    ALARM_CMD_LOCKED
	} AlarmCommand_TypeDef;

typedef struct {
    AlarmCommand_TypeDef command;
    uint16_t duration;      /* Duration in ms */
    uint8_t pattern;         /* Number of repetitions */
} AlarmMessage_TypeDef;

void Buzzer_Beep(uint16_t duration_ms);
void Buzzer_BeepPattern(uint8_t pattern);
void LED_Blink(uint16_t on_time, uint16_t off_time, uint8_t count);
void LED_SetState(uint8_t state);
void BUZZER_ON(void);
void LED_ON(void);
void BUZZER_OFF(void);
void LED_OFF(void);
#endif // ALARM_H
