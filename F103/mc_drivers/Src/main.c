/*
 * Project: Drivers for testing
 *
 * @author: Marcos Yonamine
 *
 * */

/*
 *  ATTENTION: The rotary encoder worked fine with 1uF capacitor on pins CLK and DT
 */

#include "main.h"

// ####################################################################################

/* PRIVATE GLOBAL VARIABLES */

static uint8_t debug_buffer[1024];  // buffer to send data
static tm1637_t tm1637; // TM1637 4-Digit display
//static uint8_t read_char; // received data from uart

// ####################################################################################

/* PRIVATE FUNCTIONS PROTOTYPES */

// Button 0 callback functions
static void button0_short_callback();           // callback when short pressed Button0
static void button0_long_callback();            // callback when long pressed Button0
static void button0_long_release_callback();    // callback when released after long press

// Button 1 callback functions
static void button1_short_callback();           // callback when short pressed Button1
static void button1_long_callback();            // callback when long pressed Button1
static void button1_long_release_callback();    // callback when released after long press

// Encoder functions
static void encoder_config();
static void encoder_clkwise_callback();
static void encoder_counter_clkwise_callback();
static void encoder_sw_callback();

// TM1638 Display functions
static void tm1637_config();

// ####################################################################################

int main(void)
{
    // Config System Clock to 64 MHz
    Sys_Clock_Init();

    // Config Timers
    Timer_Init();

    // Config Leds
    Leds_Config();

    // Config Encoder
    encoder_config();

    // Config BUTTONS
    Button_config(BUTTON0, BUTTON0_PORT, BUTTON0_PIN, BUTTON_ACTIVE_LOW, 20, 1000,      \
            button0_short_callback, button0_long_callback, button0_long_release_callback);

    Button_config(BUTTON1, BUTTON1_PORT, BUTTON1_PIN, BUTTON_ACTIVE_LOW, 20, 1000,      \
            button1_short_callback, button1_long_callback, button1_long_release_callback);

    // Config Buzzer
    Buzzer_Config(BUZZER_PORT, BUZZER_PIN);

    // Config Debug Uart
    Uart_config(UART_DEBUG, 115200, UART_NO_REMAP);

    // Config Protocol
    Mc_Protocol_Config();

    // Config TM1637 4-Digit Display
    tm1637_config();

    // Config my_clock
    My_Clock_Config(MODE_INCREMENT, MODE_24H);

    // Set Clock
    My_Clock_Set_HMS(20, 0, 0);

    // Start Clock
    My_Clock_Start_Counting();

    sprintf((char *)debug_buffer, "System init complete\r\n");
    debug_send_msg(debug_buffer, strlen((char *)debug_buffer));

    while(1)
    {
        Timer_SM();
        TM1637_SM(&tm1637);
        Mc_Protocol_SM();
        tm1637.d0 = (My_Clock_Get_Hour() / 10);
        tm1637.d1 = (My_Clock_Get_Hour() % 10);
        tm1637.d2 = (My_Clock_Get_Minute() / 10);
        tm1637.d3 = (My_Clock_Get_Minute() % 10);



    }// end while (1)

}// end main

/* PRIVATE FUNCTIONS */

static void button0_short_callback()
{
    Leds_Red_Led_Toggle();
}
static void button0_long_callback()
{
    static uint8_t blinking_flag = 0;
    if(blinking_flag)
    {
        Timer_Stop(TIMER_GREEN_LED);
        Leds_Green_Led_OFF();
        blinking_flag = 0;
    }
    else
    {
        Timer_Set(TIMER_GREEN_LED, TIME_500MS, Leds_Green_Led_Toggle, TIMER_MODE_ALWAYS);
        blinking_flag = 1;
    }
}
static void button0_long_release_callback()
{

}

static void button1_short_callback()
{
    Buzzer_short_beep(4);
}
static void button1_long_callback()
{
    Buzzer_on();
}
static void button1_long_release_callback()
{
    Buzzer_off();
}



static void encoder_clkwise_callback()
{
    My_Clock_Increment_Minute();
}

static void encoder_counter_clkwise_callback()
{
    My_Clock_Decrement_Minute();
}


static void encoder_sw_callback()
{
    My_Clock_Set_Seconds(0);
}

static void encoder_config()
{
    Encoder_Config_callbacks(encoder_clkwise_callback, encoder_counter_clkwise_callback, encoder_sw_callback);

    Encoder_Config_clk(ENCODER_CLK_PORT, ENCODER_CLK_PIN);
    Encoder_Config_dt(ENCODER_DT_PORT, ENCODER_DT_PIN);
    Encoder_Config_sw(ENCODER_SW_PORT, ENCODER_SW_PIN);

    Encoder_Init();
}


static void tm1637_config()
{
    tm1637.CLK.GPIOx = TM1637_CLK_PORT;
    tm1637.CLK.Pin = TM1637_CLK_PIN;
    tm1637.DIO.GPIOx = TM1637_DIO_PORT;
    tm1637.DIO.Pin = TM1637_DIO_PIN;
    tm1637.d0 = 0;
    tm1637.d1 = 0;
    tm1637.d2 = 0;
    tm1637.d3 = 0;
    tm1637.dot = 1;                     // Display dots
    tm1637.dot_blink_on = 1;            // Dots blinking
    tm1637.hours_digits_on = 1;         // Display hours digits
    tm1637.hours_digits_blink_on = 0;   // Hours digits not blinking
    tm1637.minutes_digits_on = 1;       // Display minutes digits
    tm1637.minutes_digits_blink_on = 0; // Minutes digits not blinking

    TM1637_Init(&tm1637);
    TM1637_Set_Brightness(&tm1637, 8); // set max brightness

}

// ####################################################################################

