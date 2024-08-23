/*
 * exti.c
 *
 *  Created on: Aug 20, 2024
 *      Author: Mc
 */


#include "exti.h"

static uint16_t exti_used_lines_flags = 0;

exti_status_e Exti_config_source(exti_line_e line, GPIO_TypeDef *GPIO, exti_trigger_mode_e mode)
{
    // Verify if exti line is already in use
    if(exti_used_lines_flags & (1<<line))
    {
        return EXTI_STATUS_ERR_LINE_USED;
    }

    // Set the used lines flag for the exti line to be configured
    exti_used_lines_flags |= (1<<(line));

    __disable_irq();

    // Enable APB2 clock for AFIO
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;

    uint8_t port_mask = 0;

    switch( (uint32_t)GPIO)
    {
        case (uint32_t)GPIOA:
            port_mask = 0b0000;
            break;
        case (uint32_t)GPIOB:
            port_mask = 0b0001;
            break;
        case (uint32_t)GPIOC:
            port_mask = 0b0010;
            break;
        case (uint32_t)GPIOD:
            port_mask = 0b0011;
            break;
        case (uint32_t)GPIOE:
            port_mask = 0b0100;
            break;
//        case (uint32_t)GPIOF:
//            port_mask = 0b0101;
//            break;
//        case (uint32_t)GPIOG:
//            port_mask = 0b0110;
//            break;
        default:
            break;
    }// end switch GPIO

    switch(line)
    {
        case EXTI_LINE_0:
        case EXTI_LINE_1:
        case EXTI_LINE_2:
        case EXTI_LINE_3:
            AFIO->EXTICR[0] |= (port_mask) << (line*4);
            break;

        case EXTI_LINE_4:
        case EXTI_LINE_5:
        case EXTI_LINE_6:
        case EXTI_LINE_7:
            AFIO->EXTICR[1] |= (port_mask) << ( (line-4)*4 );
            break;

        case EXTI_LINE_8:
        case EXTI_LINE_9:
        case EXTI_LINE_10:
        case EXTI_LINE_11:
            AFIO->EXTICR[2] |= (port_mask) << ( (line-8)*4 );
            break;

        case EXTI_LINE_12:
        case EXTI_LINE_13:
        case EXTI_LINE_14:
        case EXTI_LINE_15:
            AFIO->EXTICR[3] |= (port_mask) << ( (line-12)*4 );
            break;

        default:
            break;
    }// end switch line

    // Config input mask
    EXTI->IMR |= (0x01)<<(line);

    // Config trigger mode
    switch(mode)
    {
        case EXTI_RISING_IT_TRIGGER:
            EXTI->RTSR |= (0x01)<<line;
            break;
        case EXTI_FALLING_IT_TRIGGER:
            EXTI->FTSR |= (0x01)<<line;
            break;
        case EXTI_RISING_FALLING_IT_TRIGGER:
            EXTI->RTSR |= (0x01)<<line;
            EXTI->FTSR |= (0x01)<<line;
            break;
        default:
            break;
    }// end switch mode


    // Enable NVIC IRQ
    switch(line)
    {
        case EXTI_LINE_0:
            __NVIC_EnableIRQ(EXTI0_IRQn);
            break;
        case EXTI_LINE_1:
            __NVIC_EnableIRQ(EXTI1_IRQn);
            break;
        case EXTI_LINE_2:
            __NVIC_EnableIRQ(EXTI2_IRQn);
            break;
        case EXTI_LINE_3:
            __NVIC_EnableIRQ(EXTI3_IRQn);
            break;
        case EXTI_LINE_4:
            __NVIC_EnableIRQ(EXTI4_IRQn);
            break;
        case EXTI_LINE_5:
        case EXTI_LINE_6:
        case EXTI_LINE_7:
        case EXTI_LINE_8:
        case EXTI_LINE_9:
            __NVIC_EnableIRQ(EXTI9_5_IRQn);
            break;
        case EXTI_LINE_10:
        case EXTI_LINE_11:
        case EXTI_LINE_12:
        case EXTI_LINE_13:
        case EXTI_LINE_14:
        case EXTI_LINE_15:
            __NVIC_EnableIRQ(EXTI15_10_IRQn);
            break;
        default:
            break;
    }

    __enable_irq(); // enables all interrupts

    return EXTI_STATUS_OK;

}// end Exti_config_source


/* Callback functions pointers */
void (*Exti_callback_line_0)(void);
void (*Exti_callback_line_1)(void);
void (*Exti_callback_line_2)(void);
void (*Exti_callback_line_3)(void);
void (*Exti_callback_line_4)(void);
void (*Exti_callback_line_5)(void);
void (*Exti_callback_line_6)(void);
void (*Exti_callback_line_7)(void);
void (*Exti_callback_line_8)(void);
void (*Exti_callback_line_9)(void);
void (*Exti_callback_line_10)(void);
void (*Exti_callback_line_11)(void);
void (*Exti_callback_line_12)(void);
void (*Exti_callback_line_13)(void);
void (*Exti_callback_line_14)(void);
void (*Exti_callback_line_15)(void);

void Exti_config_callback_line(uint8_t line, Exti_CallbackFunc_t callback)
{
    switch(line)
    {
        case 0:
            Exti_callback_line_0 = callback;
            break;
        case 1:
            Exti_callback_line_1 = callback;
            break;
        case 2:
            Exti_callback_line_2 = callback;
            break;
        case 3:
            Exti_callback_line_3 = callback;
            break;
        case 4:
            Exti_callback_line_4 = callback;
            break;
        case 5:
            Exti_callback_line_5 = callback;
            break;
        case 6:
            Exti_callback_line_6 = callback;
            break;
        case 7:
            Exti_callback_line_7 = callback;
            break;
        case 8:
            Exti_callback_line_8 = callback;
            break;
        case 9:
            Exti_callback_line_9 = callback;
            break;
        case 10:
            Exti_callback_line_10 = callback;
            break;
        case 11:
            Exti_callback_line_11 = callback;
            break;
        case 12:
            Exti_callback_line_12 = callback;
            break;
        case 13:
            Exti_callback_line_13 = callback;
            break;
        case 14:
            Exti_callback_line_14 = callback;
            break;
        case 15:
            Exti_callback_line_15 = callback;
            break;

    }
}

void EXTI0_IRQHandler()
{
    // Clear Pendig Register bit
    EXTI->PR |= EXTI_PR_PR0;
    Exti_callback_line_0();

}

void EXTI1_IRQHandler()
{
    EXTI->PR |= EXTI_PR_PR1;
    Exti_callback_line_1();
}

void EXTI2_IRQHandler()
{
    EXTI->PR |= EXTI_PR_PR2;
    Exti_callback_line_2();
}

void EXTI3_IRQHandler()
{
    EXTI->PR |= EXTI_PR_PR3;
    Exti_callback_line_3();
}

void EXTI4_IRQHandler()
{
    EXTI->PR |= EXTI_PR_PR4;
    Exti_callback_line_4();
}

void EXTI9_5_IRQHandler()
{
    switch( (uint32_t)EXTI->PR )
    {
        case EXTI_PR_PR5:
            EXTI->PR |= EXTI_PR_PR5;
            Exti_callback_line_5();
            break;
        case EXTI_PR_PR6:
            EXTI->PR |= EXTI_PR_PR6;
            Exti_callback_line_6();
            break;
        case EXTI_PR_PR7:
            EXTI->PR |= EXTI_PR_PR7;
            Exti_callback_line_7();
            break;
        case EXTI_PR_PR8:
            EXTI->PR |= EXTI_PR_PR8;
            Exti_callback_line_8();
            break;
        case EXTI_PR_PR9:
            EXTI->PR |= EXTI_PR_PR9;
            Exti_callback_line_9();
            break;
    }
}

void EXTI15_10_IRQHandler()
{
    switch( (uint32_t)EXTI->PR )
    {
        case EXTI_PR_PR10:
            EXTI->PR |= EXTI_PR_PR10;
            Exti_callback_line_10();
            break;
        case EXTI_PR_PR11:
            EXTI->PR |= EXTI_PR_PR11;
            Exti_callback_line_11();
            break;
        case EXTI_PR_PR12:
            EXTI->PR |= EXTI_PR_PR12;
            Exti_callback_line_12();
            break;
        case EXTI_PR_PR13:
            EXTI->PR |= EXTI_PR_PR13;
            Exti_callback_line_13();
            break;
        case EXTI_PR_PR14:
            EXTI->PR |= EXTI_PR_PR14;
            Exti_callback_line_14();
            break;
        case EXTI_PR_PR15:
            EXTI->PR |= EXTI_PR_PR15;
            Exti_callback_line_15();
            break;
    }
}

void Exti_Disable_All_Lines()
{
    __NVIC_DisableIRQ(EXTI0_IRQn);
    __NVIC_DisableIRQ(EXTI1_IRQn);
    __NVIC_DisableIRQ(EXTI3_IRQn);
    __NVIC_DisableIRQ(EXTI4_IRQn);
    __NVIC_DisableIRQ(EXTI9_5_IRQn);
    __NVIC_DisableIRQ(EXTI15_10_IRQn);
}
