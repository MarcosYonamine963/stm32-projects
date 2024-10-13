#ifndef CONVERT_H_
#define CONVERT_H_

#include <stdint.h>

void convert_uint8_t_to_hexascii(uint8_t hex_val, uint8_t *hex_ascii);
void convert_uint8_t_array_to_hexascii(uint8_t *hex_val, uint16_t nbr_of_bytes,
                                    uint8_t *hex_ascii, _Bool spacing_bytes);

void convert_uint_16_to_hexascii(uint16_t hex_val, uint8_t *hex_ascii, _Bool spacing_bytes);



#endif /* CONVERT_H_ */
