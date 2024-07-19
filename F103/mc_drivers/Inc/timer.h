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

/* Max nbr of timers */
#define TIMER_N_MAX     3

/* List of timers */
#define TIMER_ONBOARD_LED       0
#define TIMER_GREEN_LED         1
#define TIMER_RED_LED           2

/*############################################*/

/* Timer modes: use at Timer_Set function */
#define TIMER_MODE_ONCE      0      // Trigger once
#define TIMER_MODE_ALWAYS    1      // Retrigger at timeout

typedef void (*CallbackFunc_t )(void);

void Timer_Init(void);
void Timer_Set(uint8_t id, uint32_t time, CallbackFunc_t callback, uint8_t timer_mode);
uint8_t Timer_Clear(uint8_t n_timer);
void Timer_SM(void);
void Delay_ms(uint32_t t);

#endif /* TIMER_H_ */
