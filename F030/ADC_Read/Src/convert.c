#include "convert.h"

uint8_t * Find (uint8_t *s1, uint16_t tam_s1, uint8_t * s2, uint16_t tam_s2)
{
    uint8_t *pos;
    uint16_t i, j;
    uint8_t flag_no = 0;
    uint8_t flag_found = 0;

    for (i = 0; i < tam_s1; i++)
    {
        for (j = 0; j < tam_s2; j++)
        {
            if (s1[i + j] != s2[j])
            {
                flag_no = 1;
                break;
            }
            flag_no = 0;
        }
        if (flag_no == 0)
        {
            flag_found = 1;
            pos = &s1[i];
            break;
        }
    }
    if (flag_found == 0)
    {
        pos = 0;
    }
    return pos;
}


uint8_t * Find_Str (uint8_t *s1, uint16_t tam_s1, char * s2, uint16_t tam_s2)
{
    uint8_t *pos;
    uint16_t i, j;
    uint8_t flag_no = 0;
    uint8_t flag_found = 0;

    for (i = 0; i < tam_s1; i++)
    {
        for (j = 0; j < tam_s2; j++)
        {
            if (s1[i + j] != s2[j])
            {
                flag_no = 1;
                break;
            }
            flag_no = 0;
        }
        if (flag_no == 0)
        {
            flag_found = 1;
            pos = &s1[i];
            break;
        }
    }
    if (flag_found == 0)
    {
        pos = 0;
    }
    return pos;
}


uint16_t Ascii_To_Dec(uint8_t * buffer, uint8_t n)
{
    uint16_t res = 0;
    uint8_t i = 0;

    for (i=0;i<n;i++)
    {
        res = ((res*10) + (buffer[i] - '0'));
    }

    return res;
}

void Uint16_To_Ascii(uint16_t num, uint8_t * result)
{
	for (uint8_t i=1; i<=5; i++)
	{
		result[5-i] = (num%10) + '0';
		num /= 10;
	}
	result[5] = '\0';
}

void Uint8_To_Ascii(uint8_t num, uint8_t * result)
{
    for (uint8_t i=1; i<=3; i++)
    {
        result[3-i] = (num%10) + '0';
        num /= 10;
    }
    result[3] = '\0';
}


uint8_t HAscii_To_Char(uint8_t hascii)
{
	if ((hascii >= 'A') && (hascii <= 'F'))
	{
		return (hascii - 'A' + 0x0A);
	}
	else if ((hascii >= 'a') && (hascii <= 'f'))
	{
		return (hascii - 'a' + 0x0A);
	}
	else if ((hascii >= '0') && (hascii <= '9'))
	{
		return (hascii - '0');
	}
	else
	{
		return 0xFF;
	}
}

uint8_t HAscii_To_Uint8(uint8_t * hascii)
{
	return ( (HAscii_To_Char(hascii[0]) << 4) | (HAscii_To_Char(hascii[1])) );
}


uint16_t HAscii_To_Uint16(uint8_t * hascii)
{
    uint16_t value = 0;
    value = (HAscii_To_Char(hascii[0]) << 12) | (HAscii_To_Char(hascii[1]) << 8) | (HAscii_To_Char(hascii[2]) << 4) | (HAscii_To_Char(hascii[3]));
    return value;
}

uint32_t HAscii_To_Uint32(uint8_t * hascii)
{
    uint32_t value = 0;
    value = (HAscii_To_Char(hascii[0]) << 28) | (HAscii_To_Char(hascii[1]) << 24) |
            (HAscii_To_Char(hascii[2]) << 20) | (HAscii_To_Char(hascii[3]) << 16) |
            (HAscii_To_Char(hascii[4]) << 12) | (HAscii_To_Char(hascii[5]) << 8) | 
            (HAscii_To_Char(hascii[6]) << 4) | (HAscii_To_Char(hascii[7]));
    return value;
}

uint8_t Is_HAscii(uint8_t ch)
{
	if ( ((ch >= 'a')&&(ch <= 'f')) || ((ch >= 'A')&&(ch <= 'F')) || ((ch >= '0')&&(ch <= '9')))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}


uint16_t NAscii_To_Uint16(uint8_t * buffer, uint8_t len)
{
	uint16_t res = 0;

	for (uint8_t i=0; i<len; i++)
	{
		res = (res*10) + (buffer[i] - '0');
	}

	return res;

}

void Uint16_To_NAscii(uint16_t num, uint8_t * result)
{
	for (uint8_t i=1; i<=5; i++)
	{
		result[5-i] = (num%10) + '0';
		num /= 10;
	}
	result[5] = '\0';
}

uint32_t NAscii_To_Uint32(uint8_t * ascii, uint8_t len)
{
	uint32_t res = 0;
	for (uint8_t i=0; i<len; i++)
	{
		res = (res*10) + (ascii[i] - '0');
	}
	return res;
}

void Uint32_To_NAscii(uint32_t num, uint8_t * result)
{
	for (uint8_t i=1; i<=10; i++)
	{
		result[10-i] = (num%10) + '0';
		num /= 10;
	}
	result[10] = '\0';
}

void Uint32_To_SNAscii(uint32_t num, uint8_t * result, uint8_t signal, \
    uint8_t bits)
{
    if (1 == signal)
    {
        uint32_t var = (num >> (bits - 1));
        if (0x01 == (0x01 & var))
        {
            var = (0xFFFFFFFF << bits);
            var |= num;
            var -= 1;
            var = (uint32_t)(~var);
            result[0] = '-';
            Uint32_To_NAscii(var, &result[1]);
        }
        else
        {
            Uint32_To_NAscii(num, &result[0]);
        }
    }
    else
    {
        Uint32_To_NAscii(num, &result[0]);
    }
}

uint16_t Array_Copy_Until(uint8_t * from, uint8_t * to, const char stop_char, uint16_t max_size)
{
    uint16_t i = 0;
    while ((from[i] != stop_char) && (from[i] != 0) && (i < max_size))
    {
        to[i] = from[i];
        i++;
    }
    return i;
}

uint16_t Str_Copy_Until(uint8_t * from, uint8_t * to, const char stop_char, uint16_t max_size)
{
    uint16_t i = 0;
    while ((from[i] != stop_char) && (from[i] != 0) && (i < max_size-1))
    {
        to[i] = from[i];
        i++;
    }
    to[i] = '\0';
    return i;
}

void Uint8_To_HAscii(uint8_t val, char * res)
{
    if( (val>>4) > 9 )
    {
        res[0] = (val>>4) + 0x37;
    }
    else
    {
        res[0] = (val>>4) + 0x30;
    }

    if( (val&0x0F) > 9 )
    {
        res[1] = val + 0x37;
    }
    else
    {
        res[1] = val = 0x30;
    }
}
