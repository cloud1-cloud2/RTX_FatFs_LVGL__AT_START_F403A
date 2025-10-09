#include "iic.h"
#include "delay.h"

void iic_init()
{
	 crm_periph_clock_enable(CRM_GPIOE_PERIPH_CLOCK, TRUE);
}

static void iic_delay()
{
    wk_delay_us(IIC_DELAY);
}

static void iic_sda_in(bool f)
{
    gpio_init_type gpio_init_struct;
    gpio_default_para_init(&gpio_init_struct);
    
    if(f == true)
    {     
        gpio_init_struct.gpio_mode = GPIO_MODE_INPUT;
        gpio_init_struct.gpio_pins = IIC_SDA_PIN;
        gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
        gpio_init(IIC_SDA_PORT, &gpio_init_struct);
    }else{
        gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;
        gpio_init_struct.gpio_out_type = GPIO_OUTPUT_OPEN_DRAIN;
        gpio_init_struct.gpio_pins = IIC_SDA_PIN;
        gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
        gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_MODERATE;
        gpio_init(IIC_SDA_PORT, &gpio_init_struct);
    }
}

void iic_init()
{
    //No need here
}

void iic_start_generate()
{   
    iic_sda_in(false);
    
    IIC_SDA_L;
    iic_delay();
}

void iic_stop_generate()
{
    iic_sda_in(false);
    
    IIC_SCL_L;
    iic_delay();
    
    IIC_SDA_L;
    iic_delay();
    
    IIC_SCL_H;
    iic_delay();
    
    IIC_SDA_H;
    iic_delay();
}

void iic_send_byte(uint8_t b)
{   
    iic_sda_in(false);
    
    for(uint8_t i = 0; i<=7; i++)
    {
        IIC_SCL_L;
        iic_delay();
        
        if((b&(0x80 >> i))!= 0)
        {
            IIC_SDA_H;
        }else{
            IIC_SDA_L;
        }
        iic_delay();
        
        IIC_SCL_H;
        iic_delay();
    }
}

uint8_t iic_receive_byte()
{
    iic_sda_in(true);
    
    uint8_t d;
    
    for(uint8_t i = 0; i<=7; i++)
    {
        IIC_SCL_L;
        iic_delay();
        IIC_SCL_H;
        iic_delay();
        
        if(IIC_SDA_STATE == SET)
        {
            d |= (0x80 >> i);
        }else{
            d &= ~(0x80 >> i);
        }
    }

    return d;
}

void iic_transmit_ack(bool v)
{
    iic_sda_in(false);
    
    IIC_SCL_L;
    iic_delay();
    
    if(v == true)
    {
        IIC_SDA_L;
        iic_delay();
    }else{
        IIC_SDA_H;
        iic_delay();
    }
    
    IIC_SCL_H;
    iic_delay();
}

bool iic_receive_ack()
{
    bool ack;
    
    iic_sda_in(true);
    
    IIC_SCL_L;
    iic_delay();
    
    IIC_SCL_H;
    iic_delay();
    
    if(IIC_SDA_STATE == RESET)
    {
        ack = true;
    }else{
        ack = false;
    } 
    
    return ack;
}
