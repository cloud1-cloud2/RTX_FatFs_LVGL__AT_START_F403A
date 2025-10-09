#ifndef DELAY_H
#define DELAY_H

#include "RTE_Components.h"
#include  CMSIS_device_header

void delay_init(void);
void wk_timebase_init(void);
void wk_delay_us(uint32_t delay);
void wk_delay_ms(uint32_t delay);

#endif
