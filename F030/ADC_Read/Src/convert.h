#ifndef CONVERT_H_
#define CONVERT_H_

#include <stdint.h>

uint8_t * Find (uint8_t *s1, uint16_t tam_s1, uint8_t * s2, uint16_t tam_s2);
uint8_t * Find_Str (uint8_t *s1, uint16_t tam_s1, char * s2, uint16_t tam_s2);
uint16_t Ascii_To_Dec(uint8_t * buffer, uint8_t n);
void Uint16_To_Ascii(uint16_t num, uint8_t * result);
void Uint8_To_Ascii(uint8_t num, uint8_t * result);


uint8_t HAscii_To_Char(uint8_t hascii);
uint8_t HAscii_To_Uint8(uint8_t * hascii);
uint16_t HAscii_To_Uint16(uint8_t * hascii);
uint32_t HAscii_To_Uint32(uint8_t * hascii);
uint8_t Is_HAscii(uint8_t ch);
uint16_t NAscii_To_Uint16(uint8_t * buffer, uint8_t len);
void Uint16_To_NAscii(uint16_t num, uint8_t * result);
uint32_t NAscii_To_Uint32(uint8_t * ascii, uint8_t len);
void Uint32_To_NAscii(uint32_t num, uint8_t * result);

uint16_t Array_Copy_Until(uint8_t * from, uint8_t * to, const char stop_char, uint16_t max_size);
uint16_t Str_Copy_Until(uint8_t * from, uint8_t * to, const char stop_char, uint16_t max_size);

void Uint8_To_HAscii(uint8_t val, char * res);
#endif /* CONVERT_H_ */
