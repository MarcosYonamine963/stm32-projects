#include "stm32f1xx.h"
#include "sys_clock.h"

void Sys_Clock_Init(void)
{

    /* Configure System Clock to 64 MHz with external crystal oscillator */

    /* Steps to configure System Clock */
    /*
       1. Enable HSE and wait to become ready;
       2. Set the Power Enable bit;
       3. Configure the Flash Latency for the clock frequency;
       4. Configure the prescalers of HCLK, PCLK1 and PCLK2;
       5. Configure PLL;
       6. Enable PLL and wait to become ready;
       7. Select the Clock Source and wait to become ready.
    */

    /* 1. Uncomment to Enable HSE and wait for HSE to become ready */
//    RCC->CR |= RCC_CR_HSEON;
//    while( !(RCC->CR & RCC_CR_HSERDY) );// when done, HSERDY is set
    /* 1. Uncomment to Enable HSI and wait for HSI to become ready */
    RCC->CR |= RCC_CR_HSION;
    while( !(RCC->CR & RCC_CR_HSIRDY) );// when done, HSERDY is set

    /* 2. Set the Power Enable bit */
    RCC->APB1ENR |= RCC_APB1ENR_PWREN;

    /* 3. Configure the FLASH PREFETCH and the LATENCY Related Settings */
    /*
       FLASH_ACR_LATENCY_0 if 0 < SYSCLK <= 24 MHz
       FLASH_ACR_LATENCY_1 if 24 MHz < SYSCLK <= 48 MHz
       FLASH_ACR_LATENCY_2 if 48 MHz < SYSCLK <= 72 MHz
     */
#if SYS_CLOCK == CLOCK_8MHz
    FLASH->ACR |= FLASH_ACR_LATENCY_0;
#elif SYS_CLOCK == CLOCK_16MHz
    FLASH->ACR |= FLASH_ACR_LATENCY_0;
#elif SYS_CLOCK == CLOCK_32MHz
    FLASH->ACR |= FLASH_ACR_LATENCY_1;
#elif SYS_CLOCK == CLOCK_64MHz
    FLASH->ACR |= FLASH_ACR_LATENCY_2;
#endif

    /* 4. Configure the PRESCALERS HCLK, PCLK1, PCLK2 */

    // AHB Prescaler
    RCC->CFGR |= RCC_CFGR_HPRE_DIV1;

    // APB1 Prescaler
    RCC->CFGR |= RCC_CFGR_PPRE1_DIV2;

    // APB2 Prescaler
    RCC->CFGR |= RCC_CFGR_PPRE2_DIV1;


    /* 5. Configure the MAIN PLL */
    /*
         HSE External Crystal Clock: 8 MHz
         HSI Oscillator clock: 8 MHz
         PLL input div clock: DIV1 (HSE)
         PLL input clock: HSI/2
         PLL internal mult clock: MULL16
         AHB presc.: 1
         APB1 presc: min: 2: max APB1 clock: 32 MHz
         APB2 presc: 1       max APB2 clock: 64 MHz
     */
//    RCC->CFGR |= RCC_CFGR_PLLSRC;               // Uncomment to select HSE as PLL input clock (default: HSI)
//    RCC->CFGR |= RCC_CFGR_PLLXTPRE_HSE;         // Do not divide HSE input clock
//    RCC->CFGR |= RCC_CFGR_PLLXTPRE_HSE_DIV2;    // Divide by 2 HSE input clock

#if SYS_CLOCK == CLOCK_8MHz
    RCC->CFGR |= RCC_CFGR_PLLMULL2;             // Multiply PLL internal clock: (HSI/2)*PLLMULL
#elif SYS_CLOCK == CLOCK_16MHz
    RCC->CFGR |= RCC_CFGR_PLLMULL4;             // Multiply PLL internal clock: (HSI/2)*PLLMULL
#elif SYS_CLOCK == CLOCK_32MHz
    RCC->CFGR |= RCC_CFGR_PLLMULL8;             // Multiply PLL internal clock: (HSI/2)*PLLMULL
#elif SYS_CLOCK == CLOCK_64MHz
    RCC->CFGR |= RCC_CFGR_PLLMULL16;             // Multiply PLL internal clock: (HSI/2)*PLLMULL
#endif

    /* 6. Enable PLL and wait for PLL to become ready */
    RCC->CR |= RCC_CR_PLLON;
    while( !(RCC->CR & RCC_CR_PLLRDY) );

    /* 7. Select the System Clock Source and wait to become ready */
    RCC->CFGR |= RCC_CFGR_SW_PLL;               // Select PLL as System Clock
    while( (RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL );

}
