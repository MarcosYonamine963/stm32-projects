#include "button.h"
#include "gpio.h"
#include "exti.h"
#include "timer.h"
#include "uart.h"
#include "params.h"
#include "stdio.h"
#include "string.h"

/* ================================================================= */
/* ==================   DEFINES AND TYPEDEFS   ===================== */

typedef struct
{
    GPIO_TypeDef *GPIO;
    uint8_t PIN;
    button_active_level_e active_level;
    uint8_t deb_time;
    uint32_t long_press_time;
    Button_CallbackFunc_t short_press_callback;
    Button_CallbackFunc_t long_press_callback;
    Button_CallbackFunc_t long_press_release_callback;
    button_state_e actual_state;
    button_state_e prev_state;

}button_t;

/* ================================================================= */
/* ==============   PRIVATE FUNCTIONS PROTOTYPES   ================= */

static void __button0_exti_callback(void);
static void __button1_exti_callback(void);

static void __button0_deb_callback(void);
static void __button1_deb_callback(void);

static void __select_exti_callback(button_list_e button_id);
static void __update_state_flags(button_list_e button_id);

static void Button_Debug_Send(uint8_t *msg, uint8_t len, _Bool send_cr_lf);

/* ================================================================= */
/* ===================   PRIVATE VARIABLES  ======================== */

button_t buttons[BUTTON_N_MAX];
static uint8_t msg[128]; // Debug msg

/* ================================================================= */
/* =================   PUBLIC FUNCTIONS   ========================== */

/*
 * @brief Do nothing
 * @note Use this function on Button_config to fill callback function when you want to do nothing
 */
void Button_no_callback(void)
{
    // do nothing
}

/*
 * @brief Config the button as Input Pullup. Set the callback functions
 *
 * @param button_id [IN]: user defined ID. See enum button_list_e
 * @param GPIO [IN]: Button GPIO Port
 * @param PIN [IN]: Button GPIO Pin
 * @param active_level [IN]:
 *          BUTTON_ACTIVE_LOW: when pressed, GPIO Pin goes low
 *          BUTTON_ACTIVE_HIGH: when pressed, GPIO Pin goes high
 * @param deb_time [-]: Not used yet.
 * @param long_press_time [IN]: Time is sec. to identify a long press when button is kept pressed
 * @param short_callback [IN]: Function pointer to be called when a short press is identified
 * @param long_callback [IN]: Function pointer to be called when a long press is identified
 * @param long_press_release_callback [IN]:
 *                  Function pointer to be called when a release after a long press is identified
 * */
void Button_config(button_list_e button_id, GPIO_TypeDef *GPIO, uint8_t PIN, button_active_level_e active_level,
        uint8_t deb_time, uint32_t long_press_time,
        Button_CallbackFunc_t short_callback, Button_CallbackFunc_t long_callback, Button_CallbackFunc_t long_press_release_callback)
{
    buttons[button_id].GPIO = GPIO;
    buttons[button_id].PIN = PIN;
    buttons[button_id].active_level = active_level;
    buttons[button_id].deb_time = deb_time;
    buttons[button_id].long_press_time = long_press_time;
    buttons[button_id].short_press_callback = short_callback;
    buttons[button_id].long_press_callback = long_callback;
    buttons[button_id].long_press_release_callback = long_press_release_callback;
    buttons[button_id].actual_state = BUTTON_STATE_RELEASED;
    buttons[button_id].prev_state = BUTTON_STATE_RELEASED;

    Gpio_Config(GPIO, PIN, INPUT_PULL_UP);

    Exti_config_source(buttons[button_id].PIN, buttons[button_id].GPIO, EXTI_RISING_FALLING_IT_TRIGGER);
    __select_exti_callback(button_id);
}

/*
 * @brief Get the state of the button
 *
 * @param butto_id [IN]: User defined button ID. See button_list_e typedef
 * @retval button_state_e: BUTTON_STATE_PRESSED or BUTTON_STATE_RELEASED
 * */
button_state_e Button_get_state(button_list_e button_id)
{
    return buttons[button_id].actual_state;
}



/* ================================================================= */
/* =================   PRIVAATE FUNCTIONS   ======================== */

static void __button0_long_press_callback(void)
{
    sprintf((char *)msg, "Button 0 long press callback");
    Button_Debug_Send(msg, strlen((char *)msg), 1);
    (buttons[0].long_press_callback)();
}

static void __button0_exti_callback(void)
{
    Timer_Set(TIMER_BUTTON0_DEB, 10, __button0_deb_callback, TIMER_MODE_ONCE);
}

static void __button1_long_press_callback(void)
{
    sprintf((char *)msg, "Button 1 long press callback");
    Button_Debug_Send(msg, strlen((char *)msg), 1);
    (buttons[1].long_press_callback)();
}

static void __button1_exti_callback(void)
{
    Timer_Set(TIMER_BUTTON1_DEB, 10, __button1_deb_callback, TIMER_MODE_ONCE);
}

static void __button0_deb_callback(void)
{
    __update_state_flags(0);

    // verify if state has not changed
    if(buttons[0].actual_state == buttons[0].prev_state)
    {
        return;
    }

    // State changed
    if(buttons[0].actual_state == BUTTON_STATE_PRESSED)
    {
        Timer_Set(TIMER_BUTTON0_LONG_PRESS, buttons[0].long_press_time, __button0_long_press_callback, TIMER_MODE_ONCE);
    }
    else // Button Release
    {
        Timer_Stop(TIMER_BUTTON0_LONG_PRESS);
        if(Timer_Get_Counter(TIMER_BUTTON0_LONG_PRESS) < buttons[0].long_press_time)
        {
            sprintf((char *)msg, "Button 0 short callback");
            Button_Debug_Send(msg, strlen((char *)msg), 1);
            (buttons[0].short_press_callback)();
        }
        else
        {
            sprintf((char *)msg, "Button 0 long press release callback");
            Button_Debug_Send(msg, strlen((char *)msg), 1);
            (buttons[0].long_press_release_callback)();
        }
        Timer_Set_Counter(TIMER_BUTTON0_LONG_PRESS, 0);
    }

}

static void __button1_deb_callback(void)
{
    __update_state_flags(1);

    // verify if state has not changed
    if(buttons[1].actual_state == buttons[1].prev_state)
    {
        return;
    }

    // State changed
    if(buttons[1].actual_state == BUTTON_STATE_PRESSED)
    {
        Timer_Set(TIMER_BUTTON1_LONG_PRESS, buttons[1].long_press_time, __button1_long_press_callback, TIMER_MODE_ONCE);
    }
    else // Button Release
    {
        Timer_Stop(TIMER_BUTTON1_LONG_PRESS);
        if(Timer_Get_Counter(TIMER_BUTTON1_LONG_PRESS) < buttons[1].long_press_time)
        {
            sprintf((char *)msg, "Button 1 short callback");
            Button_Debug_Send(msg, strlen((char *)msg), 1);
            (buttons[1].short_press_callback)();
        }
        else
        {
            sprintf((char *)msg, "Button 1 long press release callback");
            Button_Debug_Send(msg, strlen((char *)msg), 1);
            (buttons[1].long_press_release_callback)();
        }
        Timer_Set_Counter(TIMER_BUTTON1_LONG_PRESS, 0);
    }

}

static void __select_exti_callback(button_list_e button_id)
{
    switch(button_id)
    {
        case 0:
            Exti_config_callback_line(buttons[button_id].PIN, __button0_exti_callback);
            break;
        case 1:
            Exti_config_callback_line(buttons[button_id].PIN, __button1_exti_callback);
            break;
        default:
            return;
    }
}

static void __update_state_flags(button_list_e button_id)
{
    buttons[button_id].prev_state = buttons[button_id].actual_state;

    switch(buttons[button_id].active_level)
    {
        case BUTTON_ACTIVE_LOW:

            if(Gpio_Digital_Read(buttons[button_id].GPIO, buttons[button_id].PIN))// released
            {
                buttons[button_id].actual_state = BUTTON_STATE_RELEASED;
            }
            else
            {
                buttons[button_id].actual_state = BUTTON_STATE_PRESSED;
            }
            break;

        case BUTTON_ACTIVE_HIGH:

            if(Gpio_Digital_Read(buttons[button_id].GPIO, buttons[button_id].PIN))// pressed
            {
                buttons[button_id].actual_state = BUTTON_STATE_PRESSED;
            }
            else
            {
                buttons[button_id].actual_state = BUTTON_STATE_RELEASED;
            }
            break;

        default:
            break;
    }// end switch active level
}



static void Button_Debug_Send(uint8_t *msg, uint8_t len, _Bool send_cr_lf)
{
    if( !( (params.debug_cfg) & (1<<0) ) )
    {
        return;
    }

    debug_send_msg(msg, len);
    if(send_cr_lf)
    {
        debug_send_msg((uint8_t *)"\r\n", strlen("\r\n"));
    }
}

