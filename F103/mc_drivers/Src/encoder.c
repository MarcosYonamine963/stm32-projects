#include "encoder.h"
#include "gpio.h"
#include "exti.h"
#include "timer.h"
#include "params.h"
#include "uart.h"
#include "stdio.h"
#include "string.h"

/* ================================================================= */
/* ==================   DEFINES AND TYPEDEFS   ===================== */

typedef struct
{
    GPIO_TypeDef *PORT;
    uint8_t       PIN;
}encoder_pin_t;

typedef struct
{
    encoder_pin_t clk;
    encoder_pin_t dt;
    encoder_pin_t sw;

    Encoder_CallbackFunc_t clk_wise_callback;
    Encoder_CallbackFunc_t counter_clk_wise_callback;
    Encoder_CallbackFunc_t sw_callback;

}encoder_t;

/* ================================================================= */
/* ==============   PRIVATE FUNCTIONS PROTOTYPES   ================= */

static void __update_enc_status(void);
static void __encoder_clk_pin_callback(void);
static void __encoder_dt_pin_callback(void);
static void __encoder_sw_pin_callback(void);
static void __encoder_sw_deb_callback(void);
static int __convert_gray_to_binary(void);

static void Encoder_Debug_Send(uint8_t *msg, uint8_t len, _Bool send_cr_lf);

/* ================================================================= */
/* ===================   PRIVATE VARIABLES  ======================== */

static encoder_t encoder;

// Variables to identify clockwise or counter-clockwise events
static int pos;
static int pos_before;
static int diff;

// Debug message buffer
static char msg[128];

/* ================================================================= */
/* =================   PUBLIC FUNCTIONS   ========================== */

/*
 * @brief Config Encoder clk pin. Set exti.
 *
 * @param GPIOx [IN]: Pointer to GPIO Port
 * @param PIN [IN]: GPIO Pin number
 * */
void Encoder_Config_clk(GPIO_TypeDef *GPIOx, uint8_t PIN)
{
    encoder.clk.PORT = GPIOx;
    encoder.clk.PIN = PIN;
    Gpio_Config(GPIOx, PIN, INPUT_FLOATING);
    Exti_config_source(PIN, GPIOx, EXTI_RISING_FALLING_IT_TRIGGER);
    Exti_config_callback_line(PIN, __encoder_clk_pin_callback);
}

/*
 * @brief Config Encoder dt pin. Set exti.
 *
 * @param GPIOx [IN]: Pointer to GPIO Port
 * @param PIN [IN]: GPIO Pin number
 * */
void Encoder_Config_dt(GPIO_TypeDef *GPIOx, uint8_t PIN)
{
    encoder.dt.PORT = GPIOx;
    encoder.dt.PIN = PIN;
    Gpio_Config(GPIOx, PIN, INPUT_FLOATING);
    Exti_config_source(PIN, GPIOx, EXTI_RISING_FALLING_IT_TRIGGER);
    Exti_config_callback_line(PIN, __encoder_dt_pin_callback);
}

/*
 * @brief Config Encoder sw pin (encoder button). Set exti.
 *
 * @param GPIOx [IN]: Pointer to GPIO Port
 * @param PIN [IN]: GPIO Pin number
 * */
void Encoder_Config_sw(GPIO_TypeDef *GPIOx, uint8_t PIN)
{
    encoder.sw.PORT = GPIOx;
    encoder.sw.PIN = PIN;
    Gpio_Config(GPIOx, PIN, INPUT_PULL_UP);
    Exti_config_source(PIN, GPIOx, EXTI_FALLING_IT_TRIGGER);
    Exti_config_callback_line(PIN, __encoder_sw_pin_callback);
}

/*
 * @brief Config Encoder callback functions
 *
 * @param clk_wise_callback [IN]: Function pointer to be called when encoder turns clockwise
 * @param counter_clk_wise_callback [IN]: Function pointer to be called when encoder turns counter-clockwise
 * @param sw_callback [IN]: Function pointer to be called when encoder button is pressed
 * */
void Encoder_Config_callbacks(  Encoder_CallbackFunc_t clk_wise_callback,           \
                                Encoder_CallbackFunc_t counter_clk_wise_callback,   \
                                Encoder_CallbackFunc_t sw_callback)
{
    encoder.clk_wise_callback = clk_wise_callback;
    encoder.counter_clk_wise_callback = counter_clk_wise_callback;
    encoder.sw_callback = sw_callback;
}

/*
 * Run this after all configs done!!!
 *
 * @brief Initialize the variables to detect correctly the first event
 */
void Encoder_Init()
{
    __update_enc_status();
}


/* ================================================================= */
/* =================   PRIVAATE FUNCTIONS   ======================== */

/*
 * @brief update clk and dt pins.
 *          diff var is used to determine if encoder rotation was
 *          clockwise or counter-clockwise
 * */
static void __update_enc_status(void)
{
    pos = __convert_gray_to_binary();

    diff = pos_before - pos;
    pos_before = pos;
}

/*
 * @brief called when exti on clk pin is triggered.
 *          Verify if encoder turned clock or counter-clockwise, and
 *          call the proper callback function
 * */
static void __encoder_clk_pin_callback(void)
{
    __update_enc_status();

    // Counter-clockwise
    if( (diff == -1) || (diff == 3) )
    {
        sprintf(msg, "Encoder Counter-Clockwise callback");
        Encoder_Debug_Send((uint8_t *)msg, strlen(msg), 1);
        (encoder.counter_clk_wise_callback)();
    }
    // Clockwise
    else if((diff == 1) || (diff == -3))
    {
        sprintf(msg, "Encoder Clockwise callback");
        Encoder_Debug_Send((uint8_t *)msg, strlen(msg), 1);
        (encoder.clk_wise_callback)();
    }
    else
    {
        // ignore
        return;
    }
}// end __encoder_clk_pin_callback

/*
 * @brief Called when exti on dt pin is triggered.
 *          Just update the status, and leave to clk pin to determine the events
 * */
static void __encoder_dt_pin_callback(void)
{
    __update_enc_status();
}

/*
 * @brief Called when exti on encoder button pin is triggered.
 *          Calls a debounce function
 * */
static void __encoder_sw_pin_callback(void)
{
    Timer_Set(TIMER_ENCODER_SW_DEB, 10, __encoder_sw_deb_callback, TIMER_MODE_ONCE);
}


/*
 * @brief Encoder button debounced function.
 *          Calls the encoder button callback function
 * */
static void __encoder_sw_deb_callback(void)
{
    if(Gpio_Digital_Read(encoder.sw.PORT, encoder.sw.PIN))
        return;

    sprintf(msg, "Encoder Button callback");
    Encoder_Debug_Send((uint8_t *)msg, strlen(msg), 1);
    (encoder.sw_callback)();
}// end __encoder_sw_deb_callback


/*
 * @brief Uses the clk and dt status as gray code, and convert them to a normal binary sequence
 *
 * As the encoder rotates, the clk and dt pins will act as a gray code sequence.
 * Converting the gray code sequence to normal binary code sequence allow uw to
 * make math operations to determine if the rotation was clockwise or counter-clockwise.
 *
 * @retval: 0, 1, 2 or 3. Represents the normal binary sequence.
 * */
static int __convert_gray_to_binary(void)
{
    int clk_status = Gpio_Digital_Read(encoder.clk.PORT, encoder.clk.PIN);
    int dt_status = Gpio_Digital_Read(encoder.dt.PORT, encoder.dt.PIN);

    if(clk_status == 0 && dt_status == 0)
    {
        return 0;
    }
    else if(clk_status == 0 && dt_status == 1)
    {
        return 1;
    }
    else if(clk_status == 1 && dt_status == 1)
    {
        return 2;
    }
    else if(clk_status == 1 && dt_status == 0)
    {
        return 3;
    }
    else // physically impossible state
        return 10;
}// end __convert_gray_to_binary

/*
 * @brief Send a debug message.
 *
 * @param msg [IN]: message buffer to be sent
 * @param len {IN]: message length
 * @param send_cr_lf: if 1: send 0x0D 0x0A at end of string
 * */
static void Encoder_Debug_Send(uint8_t *msg, uint8_t len, _Bool send_cr_lf)
{
    if( !( (params.debug_cfg) & (1<<1) ) )
    {
        return;
    }

    debug_send_msg(msg, len);
    if(send_cr_lf)
    {
        debug_send_msg((uint8_t *)"\r\n", strlen("\r\n"));
    }
}// end Encoder_Debug_Send
