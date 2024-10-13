#include "my_clock.h"
#include "rtc.h"
#include "params.h"
#include "uart.h"
#include "stdio.h"
#include "string.h"

#define DEBUG_CLOCK     0

#if DEBUG_CLOCK
#include "uart.h"
#include <string.h>
#include <stdio.h>
#endif

typedef struct
{
    uint8_t hour;
    uint8_t minute;
    uint8_t seconds;
    _Bool AM_PM;
}time_t;

typedef struct
{
    uint8_t flag_active;
    increment_mode_e increment_mode;
    format_mode_e format_mode;
}control_t;

typedef struct
{
    time_t time;
    control_t control;

}clock_t;

// Main clock struct
static clock_t clock;
static char msg[128]; // Debug message


static void My_Clock_Debug_Send_Time()
{
    uint8_t buffer[128];
    sprintf((char*)buffer, "Time: %02d:%02d:%02d", clock.time.hour, clock.time.minute, clock.time.seconds);
    debug_send_msg(buffer, strlen((char *)buffer));

    if(clock.time.AM_PM == AM)
    {
        sprintf((char*)buffer, " AM\r\n");
        debug_send_msg(buffer, strlen((char *)buffer));
    }
    else
    {
        sprintf((char*)buffer, " PM\r\n");
        debug_send_msg(buffer, strlen((char *)buffer));
    }
}

static void My_Clock_Debug_Send_Time_Seconds()
{
    if(params.debug_cfg & (1<<6))
    {
        My_Clock_Debug_Send_Time();
    }
}

static  void My_Clock_Debug_Send_Time_Minutes()
{
    if(params.debug_cfg & (1<<5))
    {
        My_Clock_Debug_Send_Time();
    }
}

static void My_Clock_Debug_Seconds_Send(uint8_t *msg, uint8_t len, _Bool send_cr_lf)
{
    if( !( (params.debug_cfg) & (1<<6) ) )
    {
        return;
    }

    debug_send_msg(msg, len);
    if(send_cr_lf)
    {
        debug_send_msg((uint8_t *)"\r\n", strlen("\r\n"));
    }

}

static void My_Clock_Debug_Minutes_Send(uint8_t *msg, uint8_t len, _Bool send_cr_lf)
{
    if( !( (params.debug_cfg) & (1<<5) ) )
    {
        return;
    }

    debug_send_msg(msg, len);
    if(send_cr_lf)
    {
        debug_send_msg((uint8_t *)"\r\n", strlen("\r\n"));
    }

}


static void My_Clock_Update_AM_PM()
{
    if( (clock.time.hour == 0 || clock.time.hour == 12) && clock.time.minute == 0)
    {
        clock.time.AM_PM ^= 1;
    }
    else if ( (clock.time.hour == 11 || clock.time.hour == 23) && clock.time.minute != 0 )
    {
        clock.time.AM_PM ^= 1;
    }
}


void My_Clock_Config(increment_mode_e inc_mode, format_mode_e format_mode)
{
    RTC_Config();

    clock.time.hour = 0;
    clock.time.minute = 0;
    clock.time.seconds = 0;

    clock.control.flag_active = 0; // start inactive
    //clock.control.increment_mode = inc_mode;
    clock.control.format_mode = format_mode;

    My_Clock_Set_Increment_Mode(inc_mode);

    //prev_time = Timer_Get_Sys_Tick();
    //current_time = Timer_Get_Sys_Tick();
}

void My_Clock_Set_Increment_Mode(increment_mode_e mode)
{
    clock.control.increment_mode = mode;
    if(mode == MODE_INCREMENT)
    {
        RTC_Set_Seconds_Callback(My_Clock_Increment_Second);
    }
    else
    {
        RTC_Set_Seconds_Callback(My_Clock_Decrement_Second);
    }
}

void My_Clock_Set_Format_Mode(format_mode_e format_mode)
{
    if(format_mode == MODE_12H)
    {
        if(clock.time.hour >= 12)
        {
            clock.time.hour -= 12;
            clock.time.AM_PM = PM;
        }

    }
    else // setting to 24H
    {
        if( clock.control.format_mode == MODE_12H && clock.time.AM_PM == PM )
        {
            clock.time.hour += 12;
        }
    }

    clock.control.format_mode = format_mode;

}

void My_Clock_Start_Counting(void)
{
    clock.control.flag_active = 1;
}

void My_Clock_Stop_Counting(void)
{
    clock.control.flag_active = 0;
}

void My_Clock_Set_Hour(uint8_t hour)
{
    if(hour > 12)
    {
        clock.time.AM_PM = PM;

        if(clock.control.format_mode == MODE_12H)
            hour -= 12;
    }
    clock.time.hour = hour;
}

void My_Clock_Set_Minute(uint8_t minute)
{
    sprintf((char *)msg, "Clock Minutes set to %d", minute);
    My_Clock_Debug_Minutes_Send((uint8_t *)msg, strlen((char *)msg), 1);
    clock.time.minute = minute;
}

void My_Clock_Set_Seconds(uint8_t seconds)
{
    sprintf((char *)msg, "Clock Minutes set to %d", seconds);
    My_Clock_Debug_Seconds_Send((uint8_t *)msg, strlen((char *)msg), 1);
    clock.time.seconds = seconds;
}

void My_Clock_Set_HMS(uint8_t hour, uint8_t minute, uint8_t seconds)
{
    My_Clock_Set_Hour(hour);
    My_Clock_Set_Minute(minute);
    My_Clock_Set_Seconds(seconds);
}

void My_Clock_Increment_Second(void)
{
    static _Bool cascate_debug_send_flag = 0;

    if( !(clock.control.flag_active) )
    {
        return;
    }

    clock.time.seconds++;
    if(clock.time.seconds == 60)
    {
        clock.time.seconds = 0;
        My_Clock_Increment_Minute();

        // if debug minutes is on, cascate will occur
        if(params.debug_cfg & (1<<5))
        {
            cascate_debug_send_flag = 1;
        }
    }

    if(!cascate_debug_send_flag)
    {
        My_Clock_Debug_Send_Time_Seconds();
    }
    cascate_debug_send_flag = 0;

}

void My_Clock_Increment_Minute(void)
{
    clock.time.minute++;
    if(clock.time.minute == 60)
    {
        clock.time.minute = 0;
        My_Clock_Increment_Hour();
    }
    My_Clock_Debug_Send_Time_Minutes();

}

void My_Clock_Increment_Hour(void)
{

    clock.time.hour++;

    if(clock.control.format_mode == MODE_12H)
    {
        if(clock.time.hour == 13)
        {
            clock.time.hour = 1;
        }
    }
    else
    {
        if(clock.time.hour == 24)
        {
            clock.time.hour = 0;
        }
    }

    My_Clock_Update_AM_PM();
}

void My_Clock_Decrement_Second(void)
{
    static _Bool cascate_debug_send_flag = 0;

    if( !(clock.control.flag_active) )
    {
        return;
    }

    if(clock.time.seconds == 0)
    {
        clock.time.seconds = 59;
        My_Clock_Decrement_Minute();

        // if debug minutes is on, cascate will occur
        if(params.debug_cfg & (1<<5))
        {
            cascate_debug_send_flag = 1;
        }
    }
    else
    {
        clock.time.seconds--;
    }

    if(!cascate_debug_send_flag)
    {
        My_Clock_Debug_Send_Time_Seconds();
    }
    cascate_debug_send_flag = 0;

}

void My_Clock_Decrement_Minute(void)
{
    if(clock.time.minute == 0)
    {
        clock.time.minute = 59;
        My_Clock_Decrement_Hour();
    }
    else
    {
        clock.time.minute--;
    }

    My_Clock_Debug_Send_Time_Minutes();
}

void My_Clock_Decrement_Hour(void)
{
    if(clock.control.format_mode == MODE_12H)
    {
        if(clock.time.hour == 1)
        {
            clock.time.hour = 12;
        }
        else
        {
            clock.time.hour--;
        }
    }
    else
    {
        if(clock.time.hour == 0)
        {
            clock.time.hour = 23;
        }
        else
        {
            clock.time.hour--;
        }
    }

    My_Clock_Update_AM_PM();

}



uint8_t My_Clock_Get_Hour(void)
{
    return clock.time.hour;
}

uint8_t My_Clock_Get_Minute(void)
{
    return clock.time.minute;
}

uint8_t My_Clock_Get_Seconds(void)
{
    return clock.time.seconds;
}

AM_PM_e My_Clock_Get_AMPM(void)
{
    return clock.time.AM_PM;
}


