/*
 * timer.c
 *
 *  Created on: Jul 17, 2024
 *  Author: Mc
 */

#include "stm32f1xx.h"
#include "timer.h"
#include "sys_clock.h"  // Just to get system clock value

#define FALSE   0
#define TRUE    1

/*
 * @brief Struct to manage timers.
 */
typedef struct
{
    uint8_t flag;                       /* Activate timer */
    timer_mode_e timer_mode;            /* Timer type. TIMER_MOMDE_ALWAYS or TIMER_MODE_ONCE */
    uint32_t timeout;                   /* Timer timeout time*/
    uint32_t counter;                   /* Timer counter variable */
    Timer_CallbackFunc_t callback;      /* Callback func to execute on timeout */
}timer_t;


/**
 * @brief Timers struct array
 */
timer_t timers[TIMER_N_MAX];

volatile uint32_t systick_time = 0;     // Incremented every 1 ms
volatile uint32_t us_delay_time = 0;    // Incremented every 10 us

/**
 * @brief Initialization of the timers. Config the SysTick interrupt and
 * reset all the timers flags.
 */
void Timer_Init(void)
{
    /* https://www.keil.com/pack/doc/CMSIS_Dev/Core/html/group__system__init__gr.html */
    /* Config SysTick interrupt to 1 kHz. Callback: SysTick_Handler */
    SysTick_Config( 640 ); // Config timer to 100 KHz (inc every 10us)

    // all timers initialized stopped, until setted by the Timer_Set function
    for (uint8_t i = 0; i < TIMER_N_MAX; i++)
    {
        timers[i].flag = 0;
    }

}// end Timer_Init

/**
 * @brief SysTick interrupt handler. Called every 10 us. Increment timer counters.
 */
void SysTick_Handler(void)
{

    us_delay_time++;

    if(us_delay_time % 100 == 0) //1 ms
    {
        systick_time++;

        // increment all timer's counters
        for (uint8_t i = 0; i < TIMER_N_MAX; i++)
        {
            if(timers[i].flag)
            {
                timers[i].counter++;
            }
        }// end inc timers counter

    }// end 1 ms

}// end SysTick_Handler



/**
 * @brief Config and init a timer.
 * @param timer_id: [input] Index to timers array.
 * @param time: [input] time value in ms to timeout.
 * @param callback: [function pointer] function to be called on timeout.
 * @param type_mode: [input] Tipo de execucao do timer. ALWAYS ou ONCE.
 */
void Timer_Set(uint8_t timer_id, uint32_t time, Timer_CallbackFunc_t callback, timer_mode_e timer_mode)
{
    timers[timer_id].flag = 1;
    timers[timer_id].timeout = time;
    timers[timer_id].callback = callback;
    timers[timer_id].timer_mode = timer_mode;
    timers[timer_id].counter = 0;
}


/**
 * @brief Stop the timer.
 * @param timer_id: [input] index to timer array
 * @return boolean: status of operation. 1: success, 0: fail
 * */
_Bool Timer_Stop(uint8_t timer_id)
{
    if ((timer_id >= 0) && (timer_id < TIMER_N_MAX))
    {
        timers[timer_id].flag = 0;
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/**
 * @brief Continue timer counting from where it stopped
 * @param timer_id: [input] index to timer array
 * @return boolean: status of operation. 1: success, 0: fail
 * */
_Bool Timer_Continue(uint8_t timer_id)
{
    if ((timer_id >= 0) && (timer_id < TIMER_N_MAX))
    {
        timers[timer_id].flag = 1;
        return TRUE;
    }
    else
    {
        return FALSE;
    }

}

/**
 * @brief Restart counting from 0.
 * @param timer_id: [input] index to timer array
 * @return boolean: status of operation. 1: success, 0: fail
 **/
_Bool Timer_Restart(uint8_t timer_id)
{
    if ((timer_id >= 0) && (timer_id < TIMER_N_MAX))
    {
        timers[timer_id].flag = 1;
        timers[timer_id].counter = 0;
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/**
 * @brief Get the timer counter value
 * @param timer_id: [input] index to timer array
 * @return uint32_t: timer counter value
 * */
uint32_t Timer_Get_Counter(uint8_t timer_id)
{
    if ((timer_id >= 0) && (timer_id < TIMER_N_MAX))
    {
        return timers[timer_id].counter;
    }
    else
    {
        return FALSE;
    }
}

/**
 * @brief Set the timer counter value
 * @param timer_id: [input] index to timer array
 * @param counter: [input] counter value to set
 * @return boolean: status of operation. 1: success, 0: fail
 * */
_Bool Timer_Set_Counter(uint8_t timer_id, uint32_t counter)
{
    if ((timer_id >= 0) && (timer_id < TIMER_N_MAX))
    {
        timers[timer_id].counter = counter;
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/**
 * @brief State Machine for the timers. Check if timer is on timeout and
 * executes the callback function.
 * @note run this function on embedded loop (mandatory if using this module)
 * @note2 don't use blocking functions. It may cause wrong timing values.
 */
void Timer_SM(void)
{
    for (uint8_t i=0; i<TIMER_N_MAX; i++)
    {
        if (timers[i].flag == 1)
        {
            if (timers[i].counter >= timers[i].timeout)
            {
                if (timers[i].timer_mode == TIMER_MODE_ONCE)
                {
                    timers[i].flag = 0;
                }
                else if (timers[i].timer_mode == TIMER_MODE_ALWAYS)
                {
                    timers[i].counter = 0;
                }

                (timers[i].callback)();
            }
        }
    }
}// end Timer_SM

/**
 * @brief get system clock tick (1 ms)
 * @return uint32_t systick counter value
 *
 * @note use it like Arduino's millis() function
 * */
uint32_t Timer_Get_Sys_Tick(void)
{
    __disable_irq();
    uint32_t value = systick_time;
    __enable_irq();
    return value;
}

/**
 * @brief get 100kHz clock tick (10 us)
 * @return uint32_t us_timer counter value
 *
 * */
uint32_t Timer_Get_10us_Tick(void)
{
    return us_delay_time;
}

/**
 * @brief Delay function in milliseconds. Not recommended (this function blocks the processor)
 * @param t time in milliseconds to block the processor.
 */
void Delay_ms(uint32_t t)
{
    uint32_t time = Timer_Get_Sys_Tick();
    while(Timer_Get_Sys_Tick() < time + t)
    {
        asm volatile("nop");
    }

}// end Delay_ms

/**
 * @brief Delay function in multiple of 10 us time. Not recommended (this function blocks the processor)
 * @param t time to multiply by 10 us. Ex: it t = 5, blocks processor for 50 us.
 */
void Delay_10us(uint32_t t)
{
    uint32_t time = Timer_Get_10us_Tick();
    while(Timer_Get_10us_Tick() < time + t)
    {
        asm volatile("nop");
    }

}// end Delay_10us
