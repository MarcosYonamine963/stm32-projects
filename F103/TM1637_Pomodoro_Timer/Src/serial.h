/*
 * Module for Serial Communication
 */

#ifndef SERIAL_H_
#define SERIAL_H_

#include <stdint.h>


typedef enum
{
    SERIAL_OK,
    SERIAL_NO_DATA,
    SERIAL_BUFFER_FULL
}serial_status_e;

void Serial_Config(uint32_t baud);
serial_status_e Serial_Send_Byte(uint8_t data);
serial_status_e Serial_Send_Text(char *text);
serial_status_e Serial_Read_Data(uint8_t *data);

void Serial_Force_Send_Text(char *text);

void Serial_SM(void);

#endif /* SERIAL_H_ */
