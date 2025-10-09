#ifndef FAN_SS6285L_H
#define FAN_SS6285L_H

#include "stdint.h"

void fan_ss6285l_init(void);
extern void fan_set_speed(uint16_t val);

#endif
