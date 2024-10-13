#ifndef BUTTON_H_
#define BUTTON_H_

#include <stdint.h>
#include "stm32f1xx.h"

typedef enum
{
    BUTTON0 = 0,
    BUTTON1,

    BUTTON_N_MAX
}button_list_e;

typedef enum
{
    BUTTON_STATE_PRESSED = 0,
    BUTTON_STATE_RELEASED
}button_state_e;

typedef enum
{
    BUTTON_ACTIVE_LOW = 0,
    BUTTON_ACTIVE_HIGH
}button_active_level_e;

typedef void (*Button_CallbackFunc_t )(void);


void Button_config(button_list_e button_id, GPIO_TypeDef *GPIO, uint8_t PIN, button_active_level_e active_level,
        uint8_t deb_time, uint32_t long_press_time,
        Button_CallbackFunc_t short_callback, Button_CallbackFunc_t long_callback, Button_CallbackFunc_t long_press_release_callback);

button_state_e Button_get_state(button_list_e button_id);

void Button_no_callback(void);

#endif /* BUTTON_H_ */
