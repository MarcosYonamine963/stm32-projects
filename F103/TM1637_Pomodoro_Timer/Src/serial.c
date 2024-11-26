#include "serial.h"
#include "stm32f103drivers/uart.h"
#include "stm32f103drivers/circular_buffer.h"


/*
 * UART1 NO_REMAP:       TX/PA9, RX/PA10
 * */
#define UART_MAIN   USART1


// ===========  Variables  ============ //

static circ_buffer_t buffer_rx = {{0}, 0, 0};
static circ_buffer_t buffer_tx = {{0}, 0, 0};

// ========== Private functions Prototypes ============= //
static void Serial_rx_callback(uint8_t data);


// ========== Public Functions ======================= //

void Serial_Config(uint32_t baud)
{
    Uart_config(UART_MAIN, baud, UART_NO_REMAP, Serial_rx_callback);
}

serial_status_e Serial_Send_Byte(uint8_t data)
{
    return Buffer_Write(&buffer_tx, data);
}

serial_status_e Serial_Send_Array(uint8_t *data, uint16_t len)
{
    buffer_status_e status;
    for(uint16_t i = 0; i < len; i++)
    {
        status = Buffer_Write(&buffer_tx, data[i]);
        if(status != BUFFER_OK)
        {
            return status;
        }
    }

    return status;
}

serial_status_e Serial_Send_Text(char *text)
{
    buffer_status_e status;

    // Add text to TX buffer
    for(uint8_t i = 0; text[i] != 0x00; i++)
    {
        status = Buffer_Write(&buffer_tx, text[i]);
        if(status != BUFFER_OK)
        {
            return status;
        }
    }

    return status;
}

serial_status_e Serial_Read_Data(uint8_t *data)
{
    if(Buffer_Read(&buffer_rx, data) == BUFFER_OK)
    {
        return SERIAL_OK;
    }

    return SERIAL_NO_DATA;
}

void Serial_Force_Send_Text(char *text)
{

    for(uint8_t i = 0; text[i] != 0x00; i++)
    {
        Uart_WriteChar(UART_MAIN, text[i]);
    }

}



void Serial_SM(void)
{
    uint8_t data;

    // Transmit tx buffer
    if(Buffer_Read(&buffer_tx, &data) == BUFFER_OK)
    {
        Uart_WriteChar(UART_MAIN, data);
    }

}


// ========== Private functions ============= //


static void Serial_rx_callback(uint8_t data)
{
    Buffer_Write(&buffer_rx, data);
}

