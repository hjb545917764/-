#ifndef _KEY_H
#define _KEY_H

#include "stdint.h"
#include "gpio.h"

uint32_t switch_mode( void );
uint32_t switch_data( void );

extern uint16_t	mode_switch[4];//��ȡ���뿪�ص��ĸ�����
extern uint16_t data_switch[4];

#endif
