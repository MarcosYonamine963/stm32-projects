#ifndef TM1637_LED_DISPLAY_H_
#define TM1637_LED_DISPLAY_H_

#ifdef  __cplusplus
extern "C" {
#endif

#include "stm32f1xx.h"

typedef struct
{
    GPIO_TypeDef *GPIOx;
    uint8_t Pin;
}gpio_pin_t;

typedef struct
{
    // GPIO pins
    gpio_pin_t CLK;
    gpio_pin_t DIO;

    // Display values
    uint8_t d0; // First digit (left)
    uint8_t d1; // Second digit
    uint8_t d2; // Third digit
    uint8_t d3; // Fourth digit (right)

    // Flags (they are controlled by the functions)
    _Bool dot;                      // Dots ON or OFF
    _Bool dot_blink_on;             // if 1, blinks.
    _Bool hours_digits_on;          // Hours ON or OFF
    _Bool hours_digits_blink_on;    // if 1, blinks.
    _Bool minutes_digits_on;        // Minutes ON of OFF
    _Bool minutes_digits_blink_on;  // if 1, blinks.

    // Private vars. Do not modify them
    uint32_t __timer_dot_blink_counter;     // do not change this val. It is used on State Machine
    uint32_t __timer_hour_blink_counter;    // do not change this val. It is used on State Machine
    uint32_t __timer_minute_blink_counter;  // do not change this val. It is used on State Machine
}tm1637_t;

// HIGH LEVEL FUNCTIONS
void TM1637_Init(tm1637_t *tm1637);
void TM1637_Set_Brightness(tm1637_t *tm1637, uint8_t brightness);
void TM1637_Display_Clear(tm1637_t *tm1637);
void TM1637_Display_On(tm1637_t *tm1637);
void TM1637_Display_Number(tm1637_t *tm1637, uint16_t number);
void TM1637_Display_Clock(tm1637_t *tm1637, uint8_t H, uint8_t M);
void TM1637_Dot_Blink_On(tm1637_t *tm1637);
void TM1637_Dot_Blink_Off(tm1637_t *tm1637);
void TM1637_Dot_Blink_Set_State(tm1637_t *tm1637, _Bool state);
_Bool TM1637_Dot_Blink_Get_State(tm1637_t *tm1637);
void TM1637_Hour_Digit_Blink_On(tm1637_t *tm1637);
void TM1637_Hour_Digit_Blink_Off(tm1637_t *tm1637);
void TM1637_Hour_Digit_Blink_Set_State(tm1637_t *tm1637, _Bool state);
_Bool TM1637_Hour_Digit_Blink_Get_State(tm1637_t *tm1637);
void TM1637_Minute_Digit_Blink_On(tm1637_t *tm1637);
void TM1637_Minute_Digit_Blink_Off(tm1637_t *tm1637);
void TM1637_Minute_Digit_Blink_Set_State(tm1637_t *tm1637, _Bool state);
_Bool TM1637_Minute_Digit_Blink_Get_State(tm1637_t *tm1637);

// MAIN STATE MACHINE FUNCTION
void TM1637_SM(tm1637_t *tm1637);

// MID LEVEL FUNCTIONS
void TM1637_Dot_On(tm1637_t *tm1637);
void TM1637_Dot_Off(tm1637_t *tm1637);
void TM1637_Dot_Toggle(tm1637_t *tm1637);
void TM1637_Hour_Digit_On(tm1637_t *tm1637);
void TM1637_Hour_Digit_Off(tm1637_t *tm1637);
void TM1637_Hour_Digit_Toggle(tm1637_t *tm1637);
void TM1637_Minute_Digit_On(tm1637_t *tm1637);
void TM1637_Minute_Digit_Off(tm1637_t *tm1637);
void TM1637_Minute_Digit_Toggle(tm1637_t *tm1637);

// LOW LEVEL FUNCTIONS KEPT PRIVATE FOR THE SAKE OF ORGANIZATION


#ifdef  __cplusplus
}
#endif

#endif /* TM1637_LED_DISPLAY_H_ */
