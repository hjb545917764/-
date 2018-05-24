#ifndef _CMT_H_
#define _CMT_H_

#include "common.h"
#include <stdint.h>

void CMT_PWM_QuickInit(uint16_t frequence,uint16_t duty);
void CMT_PWM_ChangDuty(uint16_t duty);

#endif
