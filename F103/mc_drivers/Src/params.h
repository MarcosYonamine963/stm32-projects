#ifndef PARAMS_H_
#define PARAMS_H_

#include "stdint.h"

typedef struct
{
    uint16_t debug_cfg;
    uint8_t buzzer_cfg;
}params_t;

// declared on params.c
extern params_t params; // readable for all modules


#endif /* PARAMS_H_ */
