/**
 * @file circular_buffer.c
 * 
 * @brief his file provides firmware functions to use a circular buffer.
 * 
 */

#include "circular_buffer.h"

/**
 * @brief Writes a new data byte on a circular buffer.
 * 
 * @param circ_buffer_t Circular buffer that are going to receive new data.
 * @param byte Data to be written.
 * @return buffer_status_e Operation status, returns if the buffer is already
 *  full or if the data could be saved.
 */
buffer_status_e Buffer_Write(volatile circ_buffer_t* buffer, uint8_t byte)
{
	uint16_t next_index = (buffer->i_last+1);
	if (next_index>=SIZE_CIRCULAR_BUFFER)
	{
		next_index=0;
	} 
	if (next_index == buffer->i_first)
	{
		return BUFFER_FULL;
	}

	buffer->data[buffer->i_last] = byte;
	buffer->i_last = next_index;

	return BUFFER_OK;
}

/**
 * @brief Reads the oldest byte saved on a circular buffer.
 * 
 * @param circ_buffer_t Circular buffer from where data will be read.
 * @param byte Pointer to the byte that must received the read data.
 * @return buffer_status_e Operation status, returns if the buffer is empty or
 * if the data could be read.
 */
buffer_status_e Buffer_Read(volatile circ_buffer_t* buffer, uint8_t * byte)
{
	if (buffer->i_last == buffer->i_first)
	{
		return BUFFER_EMPTY;
	}

	*byte = buffer->data[buffer->i_first];
	buffer->i_first = (buffer->i_first + 1);
	if (buffer->i_first>=SIZE_CIRCULAR_BUFFER)
	{
		buffer->i_first=0;
	} 

	return BUFFER_OK;
}

/**
 * @brief Reads the newest byte saved on a circular buffer.
 * 
 * @param circ_buffer_t Circular buffer from where data will be read.
 * @param byte Pointer to the byte that must received the read data.
 * @return buffer_status_t Operation status, returns if the buffer is empty or
 * if the data could be read.
 */
buffer_status_e Buffer_Peek(volatile circ_buffer_t* buffer, uint8_t * byte)
{
    uint16_t last_index = ((SIZE_CIRCULAR_BUFFER + buffer->i_last) - 1);
    last_index &= CIRCULAR_BUFFER_MASK;

    if (buffer->i_last == buffer->i_first)
    {
        return BUFFER_EMPTY;
    }
    
    *byte = buffer->data[last_index];
    return BUFFER_OK;
}



