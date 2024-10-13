#ifndef TIMER_H_
#define TIMER_H_

#include "stdint.h"

/*############################################*/

#define TIME_10MS   10
#define TIME_50MS   50
#define TIME_100MS  100
#define TIME_500MS  500
#define TIME_1S     1000
#define TIME_2S     2000
#define TIME_5S     5000

/* List of timers */
typedef enum
{
    TIMER_BUTTON0_DEB,
    TIMER_BUTTON0_LONG_PRESS,
    TIMER_BUTTON1_DEB,
    TIMER_BUTTON1_LONG_PRESS,
    TIMER_BUZZER,
    TIMER_ONBOARD_LED,
    TIMER_GREEN_LED,
    TIMER_RED_LED,
    TIMER_ENCODER_SW_DEB,
    TIMER_TM1637_DOT_BLINK,
    TIMER_TM1637_HOUR_DIGIT_BLINK,
    TIMER_TM1637_MINUTE_DIGIT_BLINK,

    TIMER_N_MAX
}timer_list_t;
/*############################################*/

/* Timer modes: use at Timer_Set function */
typedef enum
{
    TIMER_MODE_ONCE = 0,
    TIMER_MODE_ALWAYS
}timer_mode_e;


typedef void (*Timer_CallbackFunc_t )(void);

void  Timer_Init(void);
void  Timer_Set(uint8_t timer_id, uint32_t time, Timer_CallbackFunc_t callback, timer_mode_e timer_mode);
_Bool Timer_Stop(uint8_t timer_id);
_Bool Timer_Continue(uint8_t timer_id);
_Bool Timer_Restart(uint8_t timer_id);
uint32_t Timer_Get_Counter(uint8_t timer_id);
_Bool Timer_Set_Counter(uint8_t timer_id, uint32_t counter);
void  Timer_SM(void);

uint32_t Timer_Get_Sys_Tick(void);
uint32_t Timer_Get_10us_Tick(void);

void  Delay_ms(uint32_t t);  // not recommended for using this function
void Delay_10us(uint32_t t); // not recommended for using this function

#endif /* TIMER_H_ */
