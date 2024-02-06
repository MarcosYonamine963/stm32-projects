/*
 * micro_delay.c
 *
 *  Created on: Jan 9, 2024
 *      Author: mcmachine
 */


#include "micro_delay.h"
#include "stm32g070xx.h"


uint32_t micro_get_val()
{
    uint32_t ms;
    uint32_t st;
    do
    {
        ms = micro_update_millis;
        st = SysTick->VAL;
        asm volatile("nop");
        asm volatile("nop");
    } while (ms != micro_update_millis);
    return ms * 1000 - st / ((SysTick->LOAD + 1) / 1000);
}


uint32_t micro_get_val_from_ISR()
{
    uint32_t st = SysTick->VAL;
    uint32_t pending = SCB->ICSR & SCB_ICSR_PENDSTSET_Msk;
    uint32_t ms = micro_update_millis;
    if (pending == 0)
        ms++;
    return ms * 1000 - st / ((SysTick->LOAD + 1) / 1000);
}

/*
 * Blocing function
 * */
void micro_delay(uint32_t value)
{
	static uint32_t micro_last_val = 0;
	micro_last_val = micro_get_val();
	while(micro_get_val() < micro_last_val + value);
}




