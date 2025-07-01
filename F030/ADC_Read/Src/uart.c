/**
 * @file uart.c
 * @author Marcos Yonamine
 * @date 2024-12-17
 *
 * @brief Uart Driver for STM32f030. Works for USART1 and USART2. Remap not available yet
 *
 * USART1:
 *   PA9 : TX
 *   PA10: RX
 *
 * USART2:
 *   PA2 : TX
 *   PA3 : RX
 *
 */


#include "uart.h"

#define NULL            ((void *)0x00)

Uart_RX_CallbackFunc_t  Uart1_Callback_func = NULL;
Uart_RX_CallbackFunc_t  Uart2_Callback_func = NULL;


/**
 * @brief Configura e habilita uart passando o baud-rate e a funcao de callback de recepcao dos bytes.
 *
 * Nao configura os pinos de perifericos (ex: pino de selecao RS232). Apensa uart
 *
 * @param USARTx USART1 ou USART2.
 * @param baud Baud-rate.
 * @param callback funcao de callback para recepcao dos bytes.
 *
 */
void Uart_Config(USART_TypeDef *USARTx, uint32_t baud, Uart_RX_CallbackFunc_t callback)
{

    // Liga clock na GPIOA:
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

    switch((uint32_t)USARTx)
    {
        case (uint32_t)USART1:
            // PA9 : TX
            // PA10: RX

            // liga clock na USART1
            RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

            // define PA9 como AF1
            GPIOA->AFR[1] |= (1 << GPIO_AFRH_AFRH1_Pos);
            GPIOA->MODER |= GPIO_MODER_MODER9_1;

            // define PA10 como AF1
            GPIOA->AFR[1] |= (1 << GPIO_AFRH_AFRH2_Pos);
            GPIOA->MODER |= GPIO_MODER_MODER10_1;

            // configura TX e RX da USART junto com o baud desejado
            USART1->CR1 |= USART_CR1_RE | USART_CR1_TE;
            USART1->BRR = (uint32_t) (SystemCoreClock / baud);
            USART1->CR1 |= USART_CR1_UE;
            USART1->CR1 |= USART_CR1_RXNEIE;

            // Configura a funcao de callback
            Uart1_Callback_func = callback;

            // liga interrupcao de recepcao
            NVIC_SetPriority(USART1_IRQn, 3);
            NVIC_EnableIRQ(USART1_IRQn);

        break;

        case (uint32_t)USART2:
                // PA2: TX
                // PA3: RX

                // liga clock na USART2
                RCC->APB1ENR |= RCC_APB1ENR_USART2EN;

                // define PA2 como AF1
                GPIOA->AFR[0] |= (1 << GPIO_AFRL_AFRL2_Pos);
                GPIOA->MODER |= GPIO_MODER_MODER2_1;

                // define PA3 como AF1
                GPIOA->AFR[0] |= (1 << GPIO_AFRL_AFRL3_Pos);
                GPIOA->MODER |= GPIO_MODER_MODER3_1;

                // configura TX e RX da USART junto com o baud desejado
                USART2->CR1 |= USART_CR1_RE | USART_CR1_TE;
                USART2->BRR = (uint32_t) (SystemCoreClock / baud);
                USART2->CR1 |= USART_CR1_UE;
                USART2->CR1 |= USART_CR1_RXNEIE;

                // Configura a funcao de callback
                Uart2_Callback_func = callback;

                // liga interrupcao de recepcao
                NVIC_SetPriority(USART2_IRQn, 3);
                NVIC_EnableIRQ(USART2_IRQn);

        break;

    }// end switch(USARTx)

}// end Uart_Config


/**
 * @brief Habilita uma uart.
 *
 * Funciona apenas apos a execucao da funcao Uart_Config.
 *
 * @param USARTx USART1 ou USART2.
 */
void Uart_Enable(USART_TypeDef *USARTx)
{

    switch((uint32_t)USARTx)
    {
        case (uint32_t)USART1:

            // Habilita uart
            USART1->CR1 |= USART_CR1_UE;
            USART1->CR1 |= USART_CR1_RXNEIE;

            // liga interrupcao de recepcao
            NVIC_ClearPendingIRQ(USART1_IRQn);
            NVIC_EnableIRQ(USART1_IRQn);
        break;

        case (uint32_t)USART2:

            // Habilita uart
            USART2->CR1 |= USART_CR1_UE;
            USART2->CR1 |= USART_CR1_RXNEIE;

            // liga interrupcao de recepcao
            NVIC_ClearPendingIRQ(USART2_IRQn);
            NVIC_EnableIRQ(USART2_IRQn);

        break;

    }// end switch(USARTx)

}// end Uart_Enable


/**
 * @brief Desabilita uma uart.
 *
 * Funciona somente apos a execucao da funcao Uart_Config.
 *
 * @param USARTx USART1 ou USART2.
 */
void Uart_Disable(USART_TypeDef *USARTx)
{
    switch((uint32_t)USARTx)
    {
        case (uint32_t)USART1:

            // Desabilita uart
            USART1->CR1 &= ~USART_CR1_UE;
            USART1->CR1 &= ~USART_CR1_RXNEIE;

            // Desliga interrupcao
            NVIC_ClearPendingIRQ(USART1_IRQn);
            NVIC_DisableIRQ(USART1_IRQn);
            NVIC_ClearPendingIRQ(USART1_IRQn);

        break;

        case (uint32_t)USART2:

            // Desabilita uart
            USART2->CR1 &= ~USART_CR1_UE;
            USART2->CR1 &= ~USART_CR1_RXNEIE;

            // Desliga interrupcao
            NVIC_ClearPendingIRQ(USART2_IRQn);
            NVIC_DisableIRQ(USART2_IRQn);
            NVIC_ClearPendingIRQ(USART2_IRQn);

        break;

    }// end switch(USARTx)
}


uart_status_e Uart_Write_Byte(USART_TypeDef *USARTx, uint8_t data)
{
    // escreve byte no buffer de transmissao
    USARTx->TDR = data;
    // aguarda enquanto a transmissao nao foi completada
    while (!(USARTx->ISR & USART_ISR_TC))
    {
    // TODO Create timer to prevent blocking
        // if timeout: return UART_ERR
    }

    return UART_OK;
}

uart_status_e Uart_Write_Array(USART_TypeDef *USARTx, uint8_t *buffer, uint16_t length)
{

    for(uint8_t i = 0; i < length; i++)
    {
        Uart_Write_Byte(USARTx, buffer[i]);
    }

    return UART_OK;
}

uart_status_e Uart_Write_Text(USART_TypeDef *USARTx, const char *text)
{

    while ((*text) != 0)
    {
        Uart_Write_Byte(USARTx, *text);
        text++;
    }

    return UART_OK;
}




/**
 * @brief Interrupcao da USART1 para tratar byte recebido.
 *
 */
void USART1_IRQHandler(void)
{
    uint8_t ch;

    // checa se o bit RXNE (RX not empty) esta setado
    if ((USART1->ISR & USART_ISR_RXNE) == USART_ISR_RXNE)
    {
        // byte recebido, realiza leitura para limpar flag
        ch = (uint8_t)USART1->RDR;

        if(Uart1_Callback_func == NULL)
            return;

        Uart1_Callback_func(ch);
    }
}

/**
 * @brief Interrupcao da USART2 para tratar byte recebido.
 *
 */
void USART2_IRQHandler(void)
{
    uint8_t ch;

    // checa se o bit RXNE (RX not empty) esta setado
    if ((USART2->ISR & USART_ISR_RXNE) == USART_ISR_RXNE)
    {
        // byte recebido, realiza leitura para limpar flag
        ch = (uint8_t)USART2->RDR;

        if(Uart2_Callback_func == NULL)
            return;

        Uart2_Callback_func(ch);
    }
}

