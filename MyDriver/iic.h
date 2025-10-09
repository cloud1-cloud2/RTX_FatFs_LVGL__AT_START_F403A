#ifndef IIC_H
#define IIC_H

#include "RTE_Components.h"
#include  CMSIS_device_header
#include <stdbool.h>

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

void iic_init(void);
extern void iic_start_generate(void);
extern void iic_stop_generate(void);
extern void iic_send_byte(uint8_t b);
extern uint8_t iic_receive_byte(void);
extern void iic_transmit_ack(bool v);
extern bool iic_receive_ack(void);

#define AHT20_IIC_ADDR 0X70 //shifted

#endif
