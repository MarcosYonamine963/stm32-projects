#ifndef __CIRCULAR_BUFFER_H__
#define __CIRCULAR_BUFFER_H__

#include <stdint.h>

/**
 * @brief Sets the size of the circular buffer storage vector, can be changed if 
 *  a larger amount of space is required. Watch out for the RAM consumption.
 * 
 */
#define SIZE_CIRCULAR_BUFFER    0x400       //1024

/**
 * @brief Bit mask used to avoid overflow of indexes should be changed
 *  according to SIZE_CIRCULAR_BUFFER.
 * 
 */
#define CIRCULAR_BUFFER_MASK    0x3FF

/**
 * @brief Struct definition of the Circular buffer.
 * 
 */
typedef struct FIFO_Circular_Buffer
{
	uint8_t data[SIZE_CIRCULAR_BUFFER]; /**< Dara Array */
	uint16_t i_first;   /**< Index of the first position */
	uint16_t i_last;    /**< Index of the last position */
} circ_buffer_t ;

/**
 * @brief Result of operations performed on a circular buffer.
 * 
 */
typedef enum STATUS_Circular_Buffer
{
	BUFFER_OK,
	BUFFER_EMPTY,   
	BUFFER_FULL     
} buffer_status_e;

buffer_status_e Buffer_Write(volatile circ_buffer_t * buffer, uint8_t byte);
buffer_status_e Buffer_Read(volatile circ_buffer_t * buffer, uint8_t * byte);
buffer_status_e Buffer_Peek(volatile circ_buffer_t * buffer, uint8_t * byte);

#endif
