#ifndef SENSOR_AHT20_H
#define SENSOR_AHT20_H

#include "RTE_Components.h"
#include  CMSIS_device_header
#include "stdbool.h"
#include "delay.h"

#define IIC_DELAY 5

#define IIC_SCL_PORT GPIOE
#define IIC_SCL_PIN GPIO_PINS_14

#define IIC_SDA_PORT GPIOE
#define IIC_SDA_PIN GPIO_PINS_15

#define IIC_SCL_H gpio_bits_set(IIC_SCL_PORT,IIC_SCL_PIN)
#define IIC_SCL_L gpio_bits_reset(IIC_SCL_PORT,IIC_SCL_PIN)

#define IIC_SDA_H gpio_bits_set(IIC_SDA_PORT,IIC_SDA_PIN)
#define IIC_SDA_L gpio_bits_reset(IIC_SDA_PORT,IIC_SDA_PIN)

#define IIC_SCL_STATE gpio_input_data_bit_read(IIC_SCL_PORT,IIC_SCL_PIN)
#define IIC_SDA_STATE gpio_input_data_bit_read(IIC_SDA_PORT,IIC_SDA_PIN)

#define AHT20_IIC_ADDR 0X70
#define AHT20_IIC_ADDR_W 0X70
#define AHT20_IIC_ADDR_R 0X71


void sensor_aht20_init(void);
float aht20_get_temperature(void);
float aht20_get_humidity(void);

#endif
