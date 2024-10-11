#ifndef RTC_H_
#define RTC_H_

#include "stdint.h"

typedef void (*RTC_Seconds_CallbackFunc_t )(void);


void RTC_Config(void);
uint32_t RTC_Get_Counter(void);
void RTC_Set_Counter(uint32_t value);

void RTC_Set_Seconds_Callback(RTC_Seconds_CallbackFunc_t function);

#endif
