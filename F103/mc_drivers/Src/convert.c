#include "convert.h"
#include "string.h"
#include "stdio.h"

/*
 * @brief   converts pure hex val to ascii representation.
 *          Ex: 0x25 is converted to "25"
 *
 * @param hex_val [IN] hex value to be converted
 * @param hex_ascii [OUT] string in hexascii
 * */
void convert_uint8_t_to_hexascii(uint8_t hex_val, uint8_t *hex_ascii)
{
    sprintf((char *)hex_ascii, "%02X", hex_val);
}// end convert_uint8_t_to_hexascii


/*
 * @brief   converts pure hex array into ascci representation
 *          Ex: array with 01 02 03 is converted to "010203" if spacing_bytes = 0
 *          or to "01 02 03" if spacing_bytes = 1.
 *
 * @param hex_val [IN] array with hex values to be converted
 * @param nbr_of_bytes [IN] number of bytes in hex_val array
 * @param hex_ascii [OUT] string in hexascii
 * @param spacing_bytes [IN] config output format spacing
 * */
void convert_uint8_t_array_to_hexascii(uint8_t *hex_val, uint16_t nbr_of_bytes,
                                    uint8_t *hex_ascii, _Bool spacing_bytes)
{
    for(uint16_t i = 0; i < nbr_of_bytes; i++)
    {
        if(spacing_bytes)
        {
            sprintf((char *)&hex_ascii[3*i], "%02X ", hex_val[i]);
        }
        else
        {
            sprintf((char *)&hex_ascii[2*i], "%02X", hex_val[i]);
        }
    }
}// end convert_uint8_t_array_to_hexascii

/*
 * @brief   converts pure hex 16bits into ascci representation
 *          Ex: variable 0x1234 is converted to "1234" if spacing_bytes = 0
 *          or to "12 34" if spacing_bytes = 1.
 *
 * @param hex_val [IN] array with hex values to be converted
 * @param nbr_of_bytes [IN] number of bytes in hex_val array
 * @param hex_ascii [OUT] string in hexascii
 * @param spacing_bytes [IN] config output format spacing
 * */
void convert_uint_16_to_hexascii(uint16_t hex_val, uint8_t *hex_ascii, _Bool spacing_bytes)
{
    uint8_t lsb = (uint8_t)(hex_val >> 0);
    uint8_t msb = (uint8_t)(hex_val >> 8);

    if(spacing_bytes)
    {
        sprintf((char *)hex_ascii, "%02X %02X ", msb, lsb);
    }
    else
    {
        sprintf((char *)hex_ascii, "%02X%02X", msb, lsb);
    }
}// end convert_uint_16_to_hexascii
