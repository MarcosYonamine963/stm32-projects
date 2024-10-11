#include "tm1637_led_display.h"
#include "timer.h"
#include "gpio.h"

// ============================ PRIVATE FUNCTIONS ============================== //

static void tm1637_clk_set_high(tm1637_t *tm1637)
{
    Gpio_Digital_Write(tm1637->CLK.GPIOx, tm1637->CLK.Pin, 1);
}

static void tm1637_clk_set_low(tm1637_t *tm1637)
{
    Gpio_Digital_Write(tm1637->CLK.GPIOx, tm1637->CLK.Pin, 0);
}

static void tm1637_dio_set_high(tm1637_t *tm1637)
{
    Gpio_Digital_Write(tm1637->DIO.GPIOx, tm1637->DIO.Pin, 1);
}

static void tm1637_dio_set_low(tm1637_t *tm1637)
{
    Gpio_Digital_Write(tm1637->DIO.GPIOx, tm1637->DIO.Pin, 0);
}

static void tm1637_send_start_signal(tm1637_t *tm1637)
{
    tm1637_clk_set_high(tm1637);
    tm1637_dio_set_high(tm1637);
    Delay_10us(1);
    tm1637_dio_set_low(tm1637);
}

static void tm1637_send_stop_signal(tm1637_t *tm1637)
{
    tm1637_clk_set_low(tm1637);
    Delay_10us(1);
    tm1637_dio_set_low(tm1637);
    Delay_10us(1);
    tm1637_clk_set_high(tm1637);
    Delay_10us(1);
    tm1637_dio_set_high(tm1637);
}

static uint8_t tm1637_read_ack(tm1637_t *tm1637)
{
    tm1637_clk_set_low(tm1637);
    tm1637_dio_set_high(tm1637);
    Gpio_Change_Pin_Mode(tm1637->DIO.GPIOx, tm1637->DIO.Pin, INPUT_FLOATING);
    Delay_10us(1);
    uint8_t data = Gpio_Digital_Read(tm1637->DIO.GPIOx, tm1637->DIO.Pin);
    Gpio_Change_Pin_Mode(tm1637->DIO.GPIOx, tm1637->DIO.Pin, OUTPUT_OPEN_DRAIN);
    tm1637_clk_set_high(tm1637);
    Delay_10us(1);
    tm1637_clk_set_low(tm1637);
    return data;
}

static void tm1637_write_byte(tm1637_t *tm1637, uint8_t data)
{
    for(uint8_t i = 0; i < 8; i++)
    {
        tm1637_clk_set_low(tm1637);
        if(data & 0x01)
        {
            tm1637_dio_set_high(tm1637);
        }
        else
        {
            tm1637_dio_set_low(tm1637);
        }
        Delay_10us(1);
        data >>= 1;
        tm1637_clk_set_high(tm1637);
        Delay_10us(1);
    }
}

/*
 * @brief converts a 1-digit number into the respectful code to set the right segments
 *
 * @param n unsigned integer form 0 to 9. Any other value will turn the digit off (0x00).
 **/
static uint8_t tm1637_number_to_segments(uint8_t n)
{
    switch(n)
    {
        case 0:
            return 0x3F;
        case 1:
            return 0x06;
        case 2:
            return 0x5B;
        case 3:
            return 0x4F;
        case 4:
            return 0x66;
        case 5:
            return 0x6D;
        case 6:
            return 0x7D;
        case 7:
            return 0x07;
        case 8:
            return 0x7F;
        case 9:
            return 0x6F;
        default:
            return 0x00;
    }
}// end tm1637_number_to_segments

/*
 * @brief Get the values on the struct tm1637, and writes to Display
 *
 * */
static void TM1637_Display_Update(tm1637_t *tm1637)
{
    tm1637_send_start_signal(tm1637);
    tm1637_write_byte(tm1637, 0x40);
    tm1637_read_ack(tm1637);
    tm1637_send_stop_signal(tm1637);

    tm1637_send_start_signal(tm1637);
    tm1637_write_byte(tm1637, 0xc0);
    tm1637_read_ack(tm1637);

    uint8_t dot_mask = 0;
    if(tm1637->dot)
    {
        dot_mask = 0x80;
    }

    if(tm1637->hours_digits_on)
    {
        tm1637_write_byte(tm1637, tm1637_number_to_segments(tm1637->d0));
    }
    else
    {
        tm1637_write_byte(tm1637, 0x00);
    }
    tm1637_read_ack(tm1637);

    if(tm1637->hours_digits_on)
    {
        tm1637_write_byte(tm1637, tm1637_number_to_segments(tm1637->d1) | dot_mask);
    }
    else
    {
        tm1637_write_byte(tm1637, 0x00 | dot_mask);
    }
    tm1637_read_ack(tm1637);

    if(tm1637->minutes_digits_on)
    {
        tm1637_write_byte(tm1637, tm1637_number_to_segments(tm1637->d2));
    }
    else
    {
        tm1637_write_byte(tm1637, 0x00);
    }
    tm1637_read_ack(tm1637);

    if(tm1637->minutes_digits_on)
    {
        tm1637_write_byte(tm1637, tm1637_number_to_segments(tm1637->d3));
    }
    else
    {
        tm1637_write_byte(tm1637, 0x00);
    }
    tm1637_read_ack(tm1637);

    tm1637_send_stop_signal(tm1637);
}// end TM1637_Display_Update

// ============================= PUBLIC FUNCTIONS ============================== //

/*
 * @brief Display main State Machine. It handles the timers to blink the segments
 * Run this on main loop (embedded loop)
 *
 * Avoid using blocking functions
 * */
void TM1637_SM(tm1637_t *tm1637)
{
    uint32_t timer_now = Timer_Get_Sys_Tick();

    if(tm1637->dot_blink_on)
    {
        // blink at 500 ms timing rate
        if(timer_now > tm1637->__timer_dot_blink_counter + 500)
        {
            tm1637->__timer_dot_blink_counter = timer_now;
            TM1637_Dot_Toggle(tm1637);
        }
    }

    if(tm1637->hours_digits_blink_on)
    {
        // blink at 200 ms timing rate
        if(timer_now > tm1637->__timer_hour_blink_counter + 200)
        {
            tm1637->__timer_hour_blink_counter = timer_now;
            TM1637_Hour_Digit_Toggle(tm1637);
        }
    }

    if(tm1637->minutes_digits_blink_on)
    {
        // blink at 200 ms timing rate
        if(timer_now > tm1637->__timer_minute_blink_counter + 200)
        {
            tm1637->__timer_minute_blink_counter = timer_now;
            TM1637_Minute_Digit_Toggle(tm1637);
        }
    }
}// end TM1637_SM


/*
 * @brief Configure the pins and initialize it cleared.
 *
 * */
void TM1637_Init(tm1637_t *tm1637)
{
    Gpio_Config(tm1637->CLK.GPIOx, tm1637->CLK.Pin, OUTPUT_OPEN_DRAIN);
    Gpio_Config(tm1637->DIO.GPIOx, tm1637->DIO.Pin, OUTPUT_OPEN_DRAIN);
}

/*
 * @brief Set brightness
 * @param brightness 0 to 8
 * */
void TM1637_Set_Brightness(tm1637_t *tm1637, uint8_t brightness)
{
    tm1637_send_start_signal(tm1637);
    tm1637_write_byte(tm1637, brightness+0x87);
    tm1637_read_ack(tm1637);
    tm1637_send_stop_signal(tm1637);
}

/*
 * @brief Writes a 4 digit number. Dot is not changed.
 *
 * @param number a 4 digit number (from 0 to 9999)
 * */
void TM1637_Display_Number(tm1637_t *tm1637, uint16_t number)
{
    tm1637->d0 = (number%10000)/1000;
    tm1637->d1 = (number%1000)/100;
    tm1637->d2 = (number%100)/10;
    tm1637->d3 = number%10;

    TM1637_Display_Update(tm1637);
}

/*
 * @brief Display two digits value for H, and two digits value for M: HH:MM
 * Remember to set the Dots manually
 * */
void TM1637_Display_Clock(tm1637_t *tm1637, uint8_t H, uint8_t M)
{
    tm1637->d0 = (H%100)/10;
    tm1637->d1 = (H%10);
    tm1637->d2 = (M%100)/10;
    tm1637->d3 = M%10;

    TM1637_Display_Update(tm1637);
}

/*
 * @brief Clear the display (all leds off)
 * */
void TM1637_Display_Clear(tm1637_t *tm1637)
{
    tm1637->d0 = 0xFF;
    tm1637->d1 = 0xFF;
    tm1637->d2 = 0xFF;
    tm1637->d3 = 0xFF;
    tm1637->dot = 0;
    TM1637_Display_Update(tm1637);
}

/*
 * @brief Turn the Dots ON
 * */
void TM1637_Dot_On(tm1637_t *tm1637)
{
    (tm1637->dot) = 1;
    TM1637_Display_Update(tm1637);
}

/*
 * @brief Turn the Dots OFF
 * */
void TM1637_Dot_Off(tm1637_t *tm1637)
{
    (tm1637->dot) = 0;
    TM1637_Display_Update(tm1637);
}

/*
 * @brief Toggle the state of the Dots
 * */
void TM1637_Dot_Toggle(tm1637_t *tm1637)
{
    (tm1637->dot) ^= 1;
    TM1637_Display_Update(tm1637);
}

void TM1637_Hour_Digit_On(tm1637_t *tm1637)
{
    tm1637->hours_digits_on = 1;
    TM1637_Display_Update(tm1637);
}

void TM1637_Hour_Digit_Off(tm1637_t *tm1637)
{
    tm1637->hours_digits_on = 0;
    TM1637_Display_Update(tm1637);
}

void TM1637_Hour_Digit_Toggle(tm1637_t *tm1637)
{
    tm1637->hours_digits_on ^= 1;
    TM1637_Display_Update(tm1637);
}

void TM1637_Minute_Digit_On(tm1637_t *tm1637)
{
    tm1637->minutes_digits_on = 1;
    TM1637_Display_Update(tm1637);
}

void TM1637_Minute_Digit_Off(tm1637_t *tm1637)
{
    tm1637->minutes_digits_on = 0;
    TM1637_Display_Update(tm1637);
}

void TM1637_Minute_Digit_Toggle(tm1637_t *tm1637)
{
    tm1637->minutes_digits_on ^= 1;
    TM1637_Display_Update(tm1637);
}

void TM1637_Dot_Blink_On(tm1637_t *tm1637)
{
    tm1637->dot_blink_on = 1;
}

void TM1637_Dot_Blink_Off(tm1637_t *tm1637)
{
    tm1637->dot_blink_on = 0;
}

void TM1637_Hour_Digit_Blink_On(tm1637_t *tm1637)
{
    tm1637->hours_digits_blink_on = 1;
}

void TM1637_Hour_Digit_Blink_Off(tm1637_t *tm1637)
{
    tm1637->hours_digits_blink_on = 0;
}

void TM1637_Minute_Digit_Blink_On(tm1637_t *tm1637)
{
    tm1637->minutes_digits_blink_on = 1;
}

void TM1637_Minute_Digit_Blink_Off(tm1637_t *tm1637)
{
    tm1637->minutes_digits_blink_on = 0;
}
