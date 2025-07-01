#ifndef UART_H_
#define UART_H_

#ifdef  __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "stm32f0xx.h"

typedef enum
{
    UART_OK,
    UART_ERR
}uart_status_e;

// Function pointer for RX interrupt callback
typedef void (*Uart_RX_CallbackFunc_t )(uint8_t);

void Uart_Config(USART_TypeDef *USARTx, uint32_t baud, Uart_RX_CallbackFunc_t callback);

void Uart_Enable(USART_TypeDef *USARTx);
void Uart_Disable(USART_TypeDef *USARTx);

uart_status_e Uart_Write_Byte(USART_TypeDef *USARTx, uint8_t data);
uart_status_e Uart_Write_Array(USART_TypeDef *USARTx, uint8_t *buffer, uint16_t length);
uart_status_e Uart_Write_Text(USART_TypeDef *USARTx, const char *text);

#ifdef  __cplusplus
}
#endif


#endif /* UART_H_ */
