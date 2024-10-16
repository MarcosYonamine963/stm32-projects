#include "buzzer.h"
#include "gpio.h"
#include "timer.h"
#include "params.h"
#include "uart.h"
#include "stdio.h"
#include "string.h"


/*
 * Sets the Buzzer ON or OFF
 */
#define BUZZER_ON       (Gpio_Digital_Write(buzzer.GPIOx, buzzer.Pin, 1))
#define BUZZER_OFF      (Gpio_Digital_Write(buzzer.GPIOx, buzzer.Pin, 0))

/*
 * Configure Timer_Set on Buzzer_Config with the time TIME_xxMS.
 * Then, the Beep period will be: (BUZZER_TIME * TIME_xxMS * 2).
 * Ex: if configured with TIME_50MS, and BUZZER_SHORT_TIME is 5,
 * then, period is 500 ms (250 ms ON, 250 ms OFF)
 */
#define BUZZER_SHORT_TIME   2   // Period: 200 ms (100 ms ON, 100 ms OFF)
#define BUZZER_LONG_TIME    5   // Period: 500 ms (250 ms ON, 250 ms OFF)

/* States of the Buzzer State Machine */
typedef enum
{
    BUZZER_IDLE,
    BUZZER_SHORT_BEEP,
    BUZZER_LONG_BEEP,
    BUZZER_CONTINUOUS
}buzzer_state_e;

/* Buzzer struct */
typedef struct
{
    GPIO_TypeDef *GPIOx;        // gpio port
    uint8_t Pin;                // gpio pin
    buzzer_state_e state;       // actual state
    buzzer_state_e new_state;   // change to new state
    uint8_t beeps;              // configure how many times to beep
}buzzer_t;

// buzzer controlled by the functions
static buzzer_t buzzer;

// count how many beeps already beeped to compare with the configured beep times
static uint8_t beeps_counter = 0;

// Debug message buffer
static char msg[128];

// private function, called by timer on config func
static void Buzzer_Callback(void);

/*
 * @brief Send a debug message. If send_cr_lf = 1, send 0x0D 0x0A at end of string.
 *
 * @param msg [IN]: message string to send.
 * @param len [IN]: message length
 * @param send_cr_lf [IN]: 0 or 1. If 1, send 0x0D 0x0A at end of string.
 * */
static void Buzzer_Debug_Send(uint8_t *msg, uint8_t len, _Bool send_cr_lf)
{
    if( !( (params.debug_cfg) & (1<<3) ) )
    {
        return;
    }

    debug_send_msg(msg, len);
    if(send_cr_lf)
    {
        debug_send_msg((uint8_t *)"\r\n", strlen("\r\n"));
    }
}


/*
 * @brief Configure Buzzer with GPIO Port and Pin
 * @param GPIOx: pointer to GPIO Port define
 * @param Pin:   GPIO Pin number
 */
void Buzzer_Config(GPIO_TypeDef *GPIOx, uint8_t Pin)
{
    // Initialize the buzzer struct
    buzzer.GPIOx = GPIOx;
    buzzer.Pin = Pin;
    buzzer.state = BUZZER_IDLE;
    buzzer.new_state = BUZZER_IDLE;
    buzzer.beeps = 0;

    // Config buzzer pin as Output Push Pull
    Gpio_Config(GPIOx, Pin, OUTPUT_PUSH_PULL);

    // Config Timer to call the Buzzer_Callback function (state machine of the buzzer).
    // if time of 50ms is changed, the values of the defines BUZZER_SHORT_TIME and
    // BUZZER_LONG_TIME must be verified.
    Timer_Set(TIMER_BUZZER, TIME_50MS, Buzzer_Callback, TIMER_MODE_ALWAYS);
}// end Buzzer_Config

/*
 * @brief Set the buzzer state machine to shot beeps.
 *
 * @param beeps [IN]: number of beeps to beep.
 * */
void Buzzer_short_beep(uint8_t beeps)
{
    sprintf((char *)msg, "Buzzer short beep: %d", beeps);
    Buzzer_Debug_Send((uint8_t *)msg, strlen((char *)msg), 1);

    buzzer.beeps = beeps;
    beeps_counter = 0;
    buzzer.new_state = BUZZER_SHORT_BEEP;
}

/*
 * @brief Set the buzzer state machine to long beeps.
 *
 * @param beeps [IN]: number of beeps to beep.
 * */
void Buzzer_long_beep(uint8_t beeps)
{
    sprintf((char *)msg, "Buzzer long beep: %d", beeps);
    Buzzer_Debug_Send((uint8_t *)msg, strlen((char *)msg), 1);

    buzzer.beeps = beeps;
    beeps_counter = 0;
    buzzer.new_state = BUZZER_LONG_BEEP;
}

/*
 * @brief Set the buzzer state machine to continuous ON.
 *
 * */
void Buzzer_on(void)
{
    sprintf((char *)msg, "Buzzer ON");
    Buzzer_Debug_Send((uint8_t *)msg, strlen((char *)msg), 1);

    buzzer.new_state = BUZZER_CONTINUOUS;
}

/*
 * @brief Set the buzzer state machine to idle (off).
 *
 * */
void Buzzer_off(void)
{
    sprintf((char *)msg, "Buzzer OFF");
    Buzzer_Debug_Send((uint8_t *)msg, strlen((char *)msg), 1);

    buzzer.new_state = BUZZER_IDLE;
}

/*
 * Buzzer State Machine.
 * Called by timer, on Always mode (every TIME_xxMS)
 * If in beep state, time_counter is incremented.
 * Spends BUZZER_xxxx_TIME in OFF, and BUZZER_xxxx_TIME in ON,
 * where xxxx is SHORT or LONG (see defines).
 * Therefore, the Beep Period is TIME_xxMS * BUZZER_xxxx_TIME * 2.
 */
static void Buzzer_Callback(void)
{
    // represents a multiple value of TIME_xxMS (configured on Buzzer_Config)
    static uint8_t time_counter = 0;

    switch(buzzer.state)
    {
        case BUZZER_IDLE:
        default:
            if(buzzer.new_state == buzzer.state)
            {
                BUZZER_OFF;
            }
            else
            {
                buzzer.state = buzzer.new_state;
                time_counter = 0;
                beeps_counter = 0;
            }
            break;

        case BUZZER_SHORT_BEEP:

            if(buzzer.new_state != BUZZER_SHORT_BEEP)
            {
                BUZZER_OFF;
                time_counter = 0;
                beeps_counter = 0;
                buzzer.state = buzzer.new_state;
                break;
            }

            // keep doing short beeps
            if(time_counter < BUZZER_SHORT_TIME)
            {
                BUZZER_ON;
            }
            else
            {
                BUZZER_OFF;
                beeps_counter++;
            }

            time_counter++;
            if(time_counter >= BUZZER_SHORT_TIME*2)
            {
                time_counter = 0;
            }

            if(beeps_counter == buzzer.beeps*BUZZER_SHORT_TIME)
            {
                buzzer.new_state = BUZZER_IDLE;
                beeps_counter = 0;
                time_counter = 0;
                buzzer.beeps = 0;
            }

            break;

        case BUZZER_LONG_BEEP:

            if(buzzer.new_state != BUZZER_LONG_BEEP)
            {
                BUZZER_OFF;
                time_counter = 0;
                beeps_counter = 0;
                buzzer.state = buzzer.new_state;
                break;
            }

            // keep doing long beeps
            if(time_counter < BUZZER_LONG_TIME)
            {
                BUZZER_ON;
            }
            else
            {
                BUZZER_OFF;
                beeps_counter++;
            }

            time_counter++;
            if(time_counter >= BUZZER_LONG_TIME*2)
            {
                time_counter = 0;
            }

            if(beeps_counter == buzzer.beeps*BUZZER_LONG_TIME)
            {
                buzzer.new_state = BUZZER_IDLE;
                beeps_counter = 0;
                time_counter = 0;
                buzzer.beeps = 0;
            }

            break;

        case BUZZER_CONTINUOUS:

            if(buzzer.state != buzzer.new_state)
            {
                BUZZER_OFF;
                time_counter = 0;
                beeps_counter = 0;
                buzzer.state = buzzer.new_state;
                break;
            }
            BUZZER_ON;
            break;

    }// end  switch(buzzer.state)

}// end Buzzer_Callback
