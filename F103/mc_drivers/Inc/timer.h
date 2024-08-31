/*
 * timer.h
 *
 *  Created on: Jul 17, 2024
 *      Author: Mc
 */

#ifndef TIMER_H_
#define TIMER_H_

#include "stdint.h"

/*############################################*/
/* USER DEFINES */



/* List of timers */
typedef enum
{
    TIMER_ONBOARD_LED = 0,
    TIMER_GREEN_LED,
    TIMER_RED_LED,
    TIMER_BUTTON0_DEB,
    TIMER_BUTTON0_LONG_PRESS,
    TIMER_BUTTON1_DEB,
    TIMER_BUTTON1_LONG_PRESS,

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

void  Delay_ms(uint32_t t); // not recommended for using this function

#endif /* TIMER_H_ */
