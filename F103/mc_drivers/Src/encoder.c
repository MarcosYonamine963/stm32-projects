/*
 * encoder.c
 *
 *  Created on: Sep 15, 2024
 *      Author: Mc
 *
 *  see reference here:  https://www.youtube.com/watch?v=9j-y6XlaE80
 */

#include "encoder.h"
#include "gpio.h"
#include "exti.h"
#include "timer.h"

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

static encoder_t encoder;
static int pos;
static int pos_before;
static int diff;

//####################################################################

/* PRIVATE FUNCTIONS PROTOTYPES */

static void __encoder_clk_pin_callback(void);
static void __encoder_dt_pin_callback(void);
static void __encoder_sw_pin_callback(void);
static void __encoder_sw_deb_callback(void);
static int __convert_gray_to_binary(void);
static void __update_enc_status(void);

//####################################################################

/* EXPORTED FUNCTIONS */

void Encoder_Config_clk(GPIO_TypeDef *GPIOx, uint8_t PIN)
{
    encoder.clk.PORT = GPIOx;
    encoder.clk.PIN = PIN;
    Gpio_Config(GPIOx, PIN, INPUT_PULL_UP);
    Exti_config_source(PIN, GPIOx, EXTI_RISING_FALLING_IT_TRIGGER);
    Exti_config_callback_line(PIN, __encoder_clk_pin_callback);
}

void Encoder_Config_dt(GPIO_TypeDef *GPIOx, uint8_t PIN)
{
    encoder.dt.PORT = GPIOx;
    encoder.dt.PIN = PIN;
    Gpio_Config(GPIOx, PIN, INPUT_FLOATING);
    Exti_config_source(PIN, GPIOx, EXTI_RISING_FALLING_IT_TRIGGER);
    Exti_config_callback_line(PIN, __encoder_dt_pin_callback);
}


void Encoder_Config_sw(GPIO_TypeDef *GPIOx, uint8_t PIN)
{
    encoder.sw.PORT = GPIOx;
    encoder.sw.PIN = PIN;
    Gpio_Config(GPIOx, PIN, INPUT_PULL_UP);
    Exti_config_source(PIN, GPIOx, EXTI_FALLING_IT_TRIGGER);
    Exti_config_callback_line(PIN, __encoder_sw_pin_callback);
}



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
//####################################################################

/* PRIVATE FUNCTIONS */

static void __update_enc_status(void)
{
    pos = __convert_gray_to_binary();

    diff = pos_before - pos;
    pos_before = pos;
}

static void __encoder_clk_pin_callback(void)
{
    __update_enc_status();

    // Counterwise
    if( (diff == -1) || (diff == 3) )
    {
        (encoder.counter_clk_wise_callback)();
    }
    else if((diff == 1) || (diff == -3))
    {
        (encoder.clk_wise_callback)();
    }
    else
    {
        // ignore
        return;
    }
}

static void __encoder_dt_pin_callback(void)
{
    __update_enc_status();
}

static void __encoder_sw_pin_callback(void)
{
    Timer_Set(TIMER_ENCODER_SW_DEB, 10, __encoder_sw_deb_callback, TIMER_MODE_ONCE);
}



static void __encoder_sw_deb_callback(void)
{
    if(Gpio_Digital_Read(encoder.sw.PORT, encoder.sw.PIN))
        return;

    (encoder.sw_callback)();
}



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
    else
        return 10;
}


