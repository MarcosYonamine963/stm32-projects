#ifndef MY_CLOCK_H_
#define MY_CLOCK_H_

#include <stdint.h>

typedef enum
{
    MODE_INCREMENT,
    MODE_DECREMENT
}increment_mode_e;

typedef enum
{
    MODE_24H,
    MODE_12H
}format_mode_e;

typedef enum
{
    AM,
    PM
}AM_PM_e;


void My_Clock_Config(increment_mode_e inc_mode, format_mode_e format_mode);
void My_Clock_Set_Increment_Mode(increment_mode_e mode);
void My_Clock_Set_Format_Mode(format_mode_e format_mode);
void My_Clock_Start_Counting(void);
void My_Clock_Stop_Counting(void);
void My_Clock_Set_Hour(uint8_t hour);
void My_Clock_Set_Minute(uint8_t minute);
void My_Clock_Set_Seconds(uint8_t seconds);
void My_Clock_Set_HMS(uint8_t hour, uint8_t minute, uint8_t seconds);
void My_Clock_Increment_Second(void);
void My_Clock_Increment_Minute(void);
void My_Clock_Increment_Hour(void);
void My_Clock_Decrement_Second(void);
void My_Clock_Decrement_Minute(void);
void My_Clock_Decrement_Hour(void);

uint8_t My_Clock_Get_Hour(void);
uint8_t My_Clock_Get_Minute(void);
uint8_t My_Clock_Get_Seconds(void);
AM_PM_e My_Clock_Get_AMPM(void);


#endif /* MY_CLOCK_H_ */
