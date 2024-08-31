#include "button.h"
#include "gpio.h"
#include "exti.h"
#include "timer.h"

/* Private functions prototypes */
static void __button0_exti_callback(void);
static void __button1_exti_callback(void);

static void __button0_deb_callback(void);
static void __button1_deb_callback(void);

static void __select_exti_callback(button_list_e button_id);
static void __update_state_flags(button_list_e button_id);


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

button_t buttons[BUTTON_N_MAX];



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


button_state_e Button_get_state(button_list_e button_id)
{
    return buttons[button_id].actual_state;
}



/*################################################################*/
/*################   PRIVATE FUNCTIONS   #########################*/

static void __button0_exti_callback(void)
{
    Timer_Set(TIMER_BUTTON0_DEB, 10, __button0_deb_callback, TIMER_MODE_ONCE);
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
        Timer_Set(TIMER_BUTTON0_LONG_PRESS, buttons[0].long_press_time, buttons[0].long_press_callback, TIMER_MODE_ONCE);
    }
    else // Button Release
    {
        Timer_Stop(TIMER_BUTTON0_LONG_PRESS);
        if(Timer_Get_Counter(TIMER_BUTTON0_LONG_PRESS) < buttons[0].long_press_time)
        {
            (buttons[0].short_press_callback)();
        }
        else
        {
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
        Timer_Set(TIMER_BUTTON1_LONG_PRESS, buttons[1].long_press_time, buttons[1].long_press_callback, TIMER_MODE_ONCE);
    }
    else // Button Release
    {
        Timer_Stop(TIMER_BUTTON1_LONG_PRESS);
        if(Timer_Get_Counter(TIMER_BUTTON1_LONG_PRESS) < buttons[1].long_press_time)
        {
            (buttons[1].short_press_callback)();
        }
        else
        {
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





