#include "stm32f1xx.h"
#include "rtc.h"
#include "my_clock.h"

static RTC_Seconds_CallbackFunc_t callback;

/*
 * @brief Set the function to be called every second
 *
 * */
void RTC_Set_Seconds_Callback(RTC_Seconds_CallbackFunc_t function)
{
    callback = function;
}

/*
 * @brief Config RTC Clock (LSE or LSI) and enable interrupt to every second
 * */
void RTC_Config(void)
{
    // Enable Power and Backup Interface Clocks
    RCC->APB1ENR |= RCC_APB1ENR_PWREN;
    RCC->APB1ENR |= RCC_APB1ENR_BKPEN;

    // Enable access to Backup registers and RTC
    PWR->CR |= PWR_CR_DBP;

    // Select LSE as RTC Clock
    RCC->BDCR |= RCC_BDCR_RTCSEL_LSE;

    // Select LSI as RTC Clock (40 kHz)
//    RCC->BDCR |= RCC_BDCR_RTCSEL_LSI;

    // Enable RTC Clock
    RCC->BDCR |= RCC_BDCR_RTCEN;


    // Enable LSE and wait to be stable
    RCC->BDCR |= RCC_BDCR_LSEON;
    while( (RCC->BDCR & RCC_BDCR_LSERDY) != RCC_BDCR_LSERDY );

    // Enable LSI and wait to be stable (40 kHz)
//    RCC->CSR |= RCC_CSR_LSION;
//    while( (RCC->CSR & RCC_CSR_LSIRDY) != RCC_CSR_LSIRDY );


    // Configuration Procedure
    // 1. Poll RTOFF, wait until its value goes to 1
    while( (RTC->CRL & RTC_CRL_RTOFF) != RTC_CRL_RTOFF );
    // 2. Set the CNF bit to enter configuration mode
    RTC->CRL |= RTC_CRL_CNF;
    // 3. Write to registers

    // If using LSE (32.768 kHz), set prescaler to 0x7FFF (TR_CLK = 1 Hz)
    RTC->PRLL = 0x7FFF;
    // If using LSI (40 kHz), set prescaler to 0x9C3F (TR_CLK = 1 Hz)
//    RTC->PRLL = 0x9C3F;

    // Clear Seconds Interrupt Flag
    RTC->CRL &= ~(RTC_CRL_SECF);
    // Enable Seconds Interrupt
    RTC->CRH |= RTC_CRH_SECIE;

    // 4. Clear the CNF to exit the configuration mode
    RTC->CRL &= ~(RTC_CRL_CNF);
    // 5. Poll RTOFF, wait until its value goes to 1 to check the end of the write operation
    while( (RTC->CRL & RTC_CRL_RTOFF) != RTC_CRL_RTOFF );

    __NVIC_EnableIRQ(RTC_IRQn);

    // The write operation only executes when the CNF bit is cleared;
    // it takes at least three RTCCLK cycles to complete
}// end RTC_Config


uint32_t RTC_Get_Counter(void)
{
    return ((RTC->CNTH) << 16) | (RTC->CNTL);

}

void RTC_Set_Counter(uint32_t value)
{

    // Configuration Procedure
    // 1. Poll RTOFF, wait until its value goes to 1
    while( (RTC->CRL & RTC_CRL_RTOFF) != RTC_CRL_RTOFF );
    // 2. Set the CNF bit to enter configuration mode
    RTC->CRL |= RTC_CRL_CNF;
    // 3. Write to registers

    RTC->CNTH = (uint16_t)(value >> 16);
    RTC->CNTL = (uint16_t)(value);

    // 4. Clear the CNF to exit the configuration mode
    RTC->CRL &= ~(RTC_CRL_CNF);
    // 5. Poll RTOFF, wait until its value goes to 1 to check the end of the write operation
    while( (RTC->CRL & RTC_CRL_RTOFF) != RTC_CRL_RTOFF );

    // The write operation only executes when the CNF bit is cleared;
    // it takes at least three RTCCLK cycles to complete
}

/*
 * @brief   This handler is triggered every second.
 *          Call the callback function configured at RTC_Set_Seconds_Callback function
 * */
void RTC_IRQHandler(void)
{
    if(RTC->CRL & RTC_CRL_SECF)
    {
        RTC->CRL &= ~(RTC_CRL_SECF);
        callback();
    }
}
