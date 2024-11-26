#ifndef BUTTONS_H_
#define BUTTONS_H_


typedef enum
{
    BUTTON0,
    BUTTON1
}button_e;

typedef enum
{
    BUTTON_STATE_ON,
    BUTTON_STATE_OFF,
    BUTTON_STATE_UNDEFINED,
}button_state_e;

typedef void (*Button_CallbackFunc_t )(void);

void Button_Config(button_e BUTTONx, Button_CallbackFunc_t press_callback, Button_CallbackFunc_t release_callback, \
                    Button_CallbackFunc_t long_press_callback, Button_CallbackFunc_t long_release_callback);
button_state_e Button_Get_State(button_e BUTTONx);
void Button_Enable(button_e BUTTONx);
void Button_Disable(button_e BUTTONx);

#endif /* BUTTONS_H_ */
