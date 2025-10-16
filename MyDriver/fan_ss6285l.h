#ifndef FAN_SS6285L_H
#define FAN_SS6285L_H

#include "stdint.h"

#define FAN_FI TMR1_SELECT_CHANNEL_1
#define FAN_BI TMR1_SELECT_CHANNEL_1C

void fan_ss6285l_init(void);
extern void fan_set_speed(int16_t val);

#endif
