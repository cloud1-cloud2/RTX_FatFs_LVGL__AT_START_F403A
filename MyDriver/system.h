#ifndef SYSTEM_H
#define SYSTEM_H

#include "RTE_Components.h"
#include CMSIS_device_header

void system_init(void);
void system_clock_config(void);
void debug_config(void);
void nvic_config(void);
void stdout_init(void);

#endif
