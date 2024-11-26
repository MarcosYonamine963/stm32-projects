#include "buttons.h"
#include "stm32f1xx.h"
#include "stm32f103drivers/gpio.h"
#include "stm32f103drivers/exti.h"
#include "stm32f103drivers/timer.h"

#define DEBUG_OFF       0
#define DEBUG_ON        1

#define DEBUG_BUTTON    DEBUG_OFF

#if DEBUG_BUTTON
#include "stm32f103drivers/uart.h"
#endif

// Hardware connections
#define BUTTON0_GPIO    GPIOB
#define BUTTON0_PIN     14

#define BUTTON1_GPIO    GPIOB
#define BUTTON1_PIN     15


// Struct que define um botao
typedef struct
{
    GPIO_TypeDef *GPIOx;
    uint8_t PIN;
    _Bool button_enabled;
    button_state_e button_state;        // debounced state
    button_state_e button_prev_state;   // prev. debounced state
}button_t;

static button_t button0;
static button_t button1;


// Function pointers for callback functions
static Button_CallbackFunc_t button0_press_callback;
static Button_CallbackFunc_t button1_press_callback;
static Button_CallbackFunc_t button0_release_callback;
static Button_CallbackFunc_t button1_release_callback;

static Button_CallbackFunc_t button0_long_press_callback;
static Button_CallbackFunc_t button1_long_press_callback;
static Button_CallbackFunc_t button0_long_release_callback;
static Button_CallbackFunc_t button1_long_release_callback;

//================   PRIVATE FUNCTIONS   =====================//

//static void dumb_function(void)
//{
//    asm("nop");
//}

static void _button0_long_press_callback(void)
{
    #if DEBUG_BUTTON
    Uart_Transmit(USART1, (uint8_t *)"Button 0 Long Pressed\r\n", 23);
    #endif
    button0_long_press_callback();
}

static void _button1_long_press_callback(void)
{
    #if DEBUG_BUTTON
    Uart_Transmit(USART1, (uint8_t *)"Button 1 Long Pressed\r\n", 23);
    #endif
    button1_long_press_callback();
}

static void button0_deb_callback(void)
{
    button0.button_prev_state = button0.button_state;
    button0.button_state = Gpio_Digital_Read(button0.GPIOx, button0.PIN);

    // Se nao teve transicao de borda
    if(button0.button_prev_state == button0.button_state)
        return;

    // Se botao estiver desabilitado
    if(button0.button_enabled == 0)
        return;

    // Estados diferentes: teve transicao
    switch(button0.button_state)
    {
        case BUTTON_STATE_ON:
        // Button Pressed
            #if DEBUG_BUTTON
            Uart_Transmit(USART1, (uint8_t *)"Button 0 Pressed\r\n", 18);
            #endif

            (button0_press_callback)();

            Timer_Set(TIMER_BUTTON0_LONG_PRESS, TIME_2S, _button0_long_press_callback, TIMER_MODE_ONCE);

            break;
        case BUTTON_STATE_OFF:
        // Button Released
            Timer_Stop(TIMER_BUTTON0_LONG_PRESS);

            if(Timer_Get_Counter(TIMER_BUTTON0_LONG_PRESS) < TIME_2S)
            {
                #if DEBUG_BUTTON
                Uart_Transmit(USART1, (uint8_t *)"Button 0 Released\r\n", 19);
                #endif
                (button0_release_callback)();
            }
            else
            {
                #if DEBUG_BUTTON
                Uart_Transmit(USART1, (uint8_t *)"Button 0 Long press Released\r\n", 30);
                #endif
                (button0_long_release_callback)();
            }
            Timer_Set_Counter(TIMER_BUTTON0_LONG_PRESS, 0);

            break;

        case BUTTON_STATE_UNDEFINED:
        default:
            break;
    }

}

static void button1_deb_callback(void)
{
    button1.button_prev_state = button1.button_state;
    button1.button_state = Gpio_Digital_Read(button1.GPIOx, button1.PIN);

    // Se nao teve transicao de borda
    if(button1.button_prev_state == button1.button_state)
        return;

    // Se botao estiver desabilitado
    if(button1.button_enabled == 0)
        return;

    // Estados diferentes: teve transicao
    switch(button1.button_state)
    {
        case BUTTON_STATE_ON:
        // Button Pressed
            #if DEBUG_BUTTON
            Uart_Transmit(USART1, (uint8_t *)"Button 1 Pressed\r\n", 18);
            #endif

            (button0_press_callback)();

            Timer_Set(TIMER_BUTTON1_LONG_PRESS, TIME_2S, _button1_long_press_callback, TIMER_MODE_ONCE);

            break;
        case BUTTON_STATE_OFF:
        // Button Released
            Timer_Stop(TIMER_BUTTON1_LONG_PRESS);

            if(Timer_Get_Counter(TIMER_BUTTON1_LONG_PRESS) < TIME_2S)
            {
                #if DEBUG_BUTTON
                Uart_Transmit(USART1, (uint8_t *)"Button 1 Released\r\n", 19);
                #endif
                (button1_release_callback)();
            }
            else
            {
                #if DEBUG_BUTTON
                Uart_Transmit(USART1, (uint8_t *)"Button 1 Long press Released\r\n", 30);
                #endif
                (button1_long_release_callback)();
            }
            Timer_Set_Counter(TIMER_BUTTON1_LONG_PRESS, 0);

            break;

        case BUTTON_STATE_UNDEFINED:
        default:
            break;
    }

}

static void button0_exti_callback(void)
{
    Timer_Set(TIMER_BUTTON0_DEB, TIME_10MS, button0_deb_callback, TIMER_MODE_ONCE);
}

static void button1_exti_callback(void)
{
    Timer_Set(TIMER_BUTTON1_DEB, TIME_10MS, button1_deb_callback, TIMER_MODE_ONCE);
}

static void Button0_Config(void)
{

    button0.GPIOx = BUTTON0_GPIO;
    button0.PIN = BUTTON0_PIN;
    button0.button_enabled = 1;

    Gpio_Config(button0.GPIOx, button0.PIN, INPUT_PULL_UP);
    Exti_config_source(button0.PIN, button0.GPIOx, EXTI_RISING_FALLING_IT_TRIGGER);
    Exti_config_callback_line(button0.PIN, button0_exti_callback);

    button0.button_state = Gpio_Digital_Read(button0.GPIOx, button0.PIN);
    button0.button_prev_state = button0.button_state;
}

static void Button1_Config(void)
{
    button1.GPIOx = BUTTON1_GPIO;
    button1.PIN = BUTTON1_PIN;
    button1.button_enabled = 1;

    Gpio_Config(button1.GPIOx, button1.PIN, INPUT_PULL_UP);
    Exti_config_source(button1.PIN, button1.GPIOx, EXTI_RISING_FALLING_IT_TRIGGER);
    Exti_config_callback_line(button1.PIN, button1_exti_callback);

    button1.button_state = Gpio_Digital_Read(button1.GPIOx, button1.PIN);
    button1.button_prev_state = button1.button_state;
}


//================   PRIVATE FUNCTIONS   =====================//



void Button_Config(button_e BUTTONx, Button_CallbackFunc_t press_callback, Button_CallbackFunc_t release_callback, \
                    Button_CallbackFunc_t long_press_callback, Button_CallbackFunc_t long_release_callback)
{
    switch(BUTTONx)
    {
        case BUTTON0:
            Button0_Config();
            button0_press_callback = press_callback;
            button0_release_callback = release_callback;
            button0_long_press_callback = long_press_callback;
            button0_long_release_callback = long_release_callback;

            break;

        case BUTTON1:
            Button1_Config();
            button1_press_callback = press_callback;
            button1_release_callback = release_callback;
            button1_long_press_callback = long_press_callback;
            button1_long_release_callback = long_release_callback;

            break;
    }
}


button_state_e Button_Get_State(button_e BUTTONx)
{
    switch(BUTTONx)
    {
        case BUTTON0:
            return button0.button_state;
            break;

        case BUTTON1:
            return button1.button_state;
            break;

        default:
            break;
    }
    return BUTTON_STATE_UNDEFINED;
}


void Button_Enable(button_e BUTTONx)
{
    switch(BUTTONx)
    {
        case BUTTON0:
            button0.button_enabled = 1;
            break;
        case BUTTON1:
            button1.button_enabled = 1;
            break;
    }
}


void Button_Disable(button_e BUTTONx)
{
    switch(BUTTONx)
        {
            case BUTTON0:
                button0.button_enabled = 0;
                break;
            case BUTTON1:
                button1.button_enabled = 0;
                break;
        }
}
