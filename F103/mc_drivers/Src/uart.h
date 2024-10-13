#ifndef UART_H_
#define UART_H_

#include "stm32f1xx.h"


#define UART_MAIN           USART1
#define UART_DEBUG          USART3

/*
 * FOR STM32F103C8T6:
 *
 * UART1 REMAP:
 *      NO_REMAP:       TX/PA9, RX/PA10
 *      REMAP:          TX/PB6, RX/PB7
 *
 * UART2 REMAP:
 *      NO_REMAP:       TX/PA2, RX/PA3
 *      REMAP           (UNSUPPORTED FOR STM32F103C8T6)
 *
 * UART3 REMAP:
 *      NO_REMAP:       TX/PB10, RX/PB11
 *      REMAP           (UNSUPPORTED FOR STM32F103C8T6)
 * */
typedef enum
{
    UART_NO_REMAP = 0,
    UART_REMAP,         // UNSUPPORTED FOR UART2
}uart_remap_e;

typedef enum
{
    UART_OK,
    UART_ERR
}uart_status_e;

void Uart_config(USART_TypeDef *UARTx, uint32_t baud, uart_remap_e remap);
void Uart_WriteChar(USART_TypeDef *UARTx, uint8_t ch);
void Uart_Transmit(USART_TypeDef *UARTx, uint8_t *buffer, uint16_t length);

uart_status_e Uart_Read_from_buffer(USART_TypeDef *UARTx, uint8_t *read_value);

// Uart Debug function
void debug_send_msg(uint8_t *msg, uint8_t size);

#endif /* UART_H_ */
