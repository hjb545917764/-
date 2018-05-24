#ifndef _DELAY_H
#define _DELAY_H
#include "stdint.h"

#define DELAY_START 1
#define DELAY_END 0

extern uint16_t delay_flag;
extern uint16_t tt;

void PIT1_HandlerU( void );
void delay_ms( uint16_t ms );

#endif
