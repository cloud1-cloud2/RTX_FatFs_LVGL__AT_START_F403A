#ifndef VOLTAGE_ADC_H
#define VOLTAGE_ADC_H
#include "RTE_Components.h"
#include  CMSIS_device_header

extern uint16_t voltage_raw_value;

void voltage_adc_init(void); 
float voltage_adc_get_voltage_value(void);

#endif
