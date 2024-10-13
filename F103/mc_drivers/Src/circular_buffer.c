#include "circular_buffer.h"

/**
 * @brief Writes a new data byte on a circular buffer.
 * 
 * @param buffer [IN]: Circular buffer to receive the new data.
 * @param byte [IN]: Data to be written.
 * @retval buffer_status_e: Operation status, returns if the buffer is
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
}// end Buffer_Write

/**
 * @brief Reads the oldest byte saved on a circular buffer.
 * 
 * @param buffer [IN]: Circular buffer from where data will be read.
 * @param byte {OUT}: Pointer to the byte that must received the read data.
 * @retval buffer_status_e: Operation status, returns if the buffer is empty or
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
}// end Buffer_Read

/**
 * @brief Reads the newest byte saved on a circular buffer.
 * 
 * @param buffer [IN]: Circular buffer from where data will be read.
 * @param byte [OUT]: Pointer to the byte that must received the read data.
 * @retval buffer_status_e: Operation status, returns if the buffer is empty or
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
}// end Buffer_Peek
