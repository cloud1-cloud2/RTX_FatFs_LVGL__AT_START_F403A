#include "sensor_aht20.h"

void iic_init()
{
	gpio_init_type gpio_init_struct;
	
	crm_periph_clock_enable(CRM_GPIOE_PERIPH_CLOCK, TRUE);
	
  gpio_default_para_init(&gpio_init_struct);
	
	gpio_bits_set(GPIOE, GPIO_PINS_14 | GPIO_PINS_15);
	
	gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_MODERATE;
  gpio_init_struct.gpio_out_type = GPIO_OUTPUT_OPEN_DRAIN;
  gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;
  gpio_init_struct.gpio_pins = GPIO_PINS_14 | GPIO_PINS_15;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init(GPIOE, &gpio_init_struct);
}

void iic_delay()
{
    wk_delay_us(IIC_DELAY);
}

void iic_sda_in(bool f)
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


void sensor_aht20_init()
{
	iic_init();
}

float aht20_get_temperature()
{
    uint32_t temperature = 0;
    float result = 0;
    uint8_t frame_receive_buff[7];
    
    //Transmit
    iic_start_generate();
    
    iic_send_byte(AHT20_IIC_ADDR_W);
    iic_receive_ack();
    
    iic_send_byte(0xAC);
    iic_receive_ack();

    iic_send_byte(0x33);
    iic_receive_ack();
    
    iic_send_byte(0x00);
    iic_receive_ack();

    iic_stop_generate();
    
    //Receive
    iic_start_generate();
    
    iic_send_byte(AHT20_IIC_ADDR_R);
    iic_receive_ack();
        
    for(uint16_t i = 0;i<=6;i++)
    {
        if(i==6)
        {
            frame_receive_buff[i] = iic_receive_byte();
            iic_transmit_ack(false);
            break;
        }
        
        frame_receive_buff[i] = iic_receive_byte();
        iic_transmit_ack(true);
    }
    
    iic_stop_generate();
    
    
    //提取St[19:16]
    for(uint16_t i = 0;i<=3;i++)
    {
        if((frame_receive_buff[3] & (0x01 << i)) != 0)
        {
            temperature |= 0x01<<(16+i);
        }else{
            temperature &= ~(0x01<<(16+i));
        }
    }
    
    //提取St[15:8]
    for(uint16_t i = 0;i<=7;i++)
    {
        if((frame_receive_buff[4] & (0x01 << i)) != 0)
        {
            temperature |= 0x01<<(8+i);
        }else{
            temperature &= ~(0x01<<(8+i));
        }
    }
    
    //提取St[7:0]
    for(uint16_t i = 0;i<=7;i++)
    {
        if((frame_receive_buff[5] & (0x01 << i)) != 0)
        {
            temperature |= 0x01<<(0+i);
        }else{
            temperature &= ~(0x01<<(0+i));
        }
    }  
    
    //printf("Temperature: %d\n", temperature);
    result = (float)temperature/1048576*200-50;
    
    return result;
}

float aht20_get_humidity()
{
    uint32_t humidity = 0;
    float result = 0;
    
    uint8_t frame_receive_buff[7];
    
    //Transmit
    iic_start_generate();
    
    iic_send_byte(AHT20_IIC_ADDR_W);
    iic_receive_ack();
    
    iic_send_byte(0xAC);
    iic_receive_ack();

    iic_send_byte(0x33);
    iic_receive_ack();
    
    iic_send_byte(0x00);
    iic_receive_ack();

    iic_stop_generate();
    
    //Receive
    iic_start_generate();
    
    iic_send_byte(AHT20_IIC_ADDR_R);
    iic_receive_ack();
        
    for(uint16_t i = 0;i<=6;i++)
    {
        if(i==6)
        {
            frame_receive_buff[i] = iic_receive_byte();
            iic_transmit_ack(false);
            break;
        }
        
        frame_receive_buff[i] = iic_receive_byte();
        iic_transmit_ack(true);
    }
    
    iic_stop_generate();
    
    //提取Srh[19:12]
    for(uint16_t i = 0;i<=7;i++)
    {
        if((frame_receive_buff[1] & (0x01 << i)) != 0)
        {
            humidity |= 0x01<<(12+i);
        }else{
            humidity &= ~(0x01<<(12+i));
        }
    }
    
    //提取St[11:4]
    for(uint16_t i = 0;i<=7;i++)
    {
        if((frame_receive_buff[2] & (0x01 << i)) != 0)
        {
            humidity |= 0x01<<(4+i);
        }else{
            humidity &= ~(0x01<<(4+i));
        }
    }
    
    //提取St[3:0]
    for(uint16_t i = 0;i<=3;i++)
    {
        if((frame_receive_buff[3] & (0x01 << i)) != 0)
        {
            humidity |= 0x01<<(0+i);
        }else{
            humidity &= ~(0x01<<(0+i));
        }
    }  
    
    //printf("humidity: %d\n", humidity);
    result = (float)humidity/1048576;
    
    return result;
}

/*
float aht20_get_temperature()
{
    uint32_t temperature = 0;
    float result = 0;
    uint8_t frame_receive_buff[7];
    
    //Transmit
    i2c_start_generate(I2C2);
    while(i2c_flag_get(I2C2,I2C_STARTF_FLAG)!= SET);
    
    i2c_7bit_address_send(I2C2,AHT20_IIC_ADDR,I2C_DIRECTION_TRANSMIT);
    while(i2c_flag_get(I2C2,I2C_ADDR7F_FLAG)!= SET);
    i2c_flag_clear(I2C2,I2C_ADDR7F_FLAG);
    
    i2c_data_send(I2C2,0xAC);
    while(i2c_flag_get(I2C2,I2C_TDBE_FLAG)!= SET);
    
    i2c_data_send(I2C2,0x33);
    while(i2c_flag_get(I2C2,I2C_TDBE_FLAG)!= SET);

    i2c_data_send(I2C2,0x00);
    while(i2c_flag_get(I2C2,I2C_TDBE_FLAG)!= SET);
    
    while(i2c_flag_get(I2C2,I2C_TDC_FLAG)!= SET);
    
    i2c_stop_generate(I2C2);
    
    //Receive
    i2c_ack_enable(I2C2,TRUE);
    i2c_start_generate(I2C2);
    while(i2c_flag_get(I2C2,I2C_STARTF_FLAG)!= SET);
    
    i2c_7bit_address_send(I2C2,AHT20_IIC_ADDR,I2C_DIRECTION_RECEIVE);
    while(i2c_flag_get(I2C2,I2C_ADDR7F_FLAG)!= SET);
    i2c_flag_clear(I2C2,I2C_ADDR7F_FLAG);
    
    for(uint16_t i = 0;i<7;i++)
    {
        while(i2c_flag_get(I2C2,I2C_RDBF_FLAG)!= SET);
        frame_receive_buff[i] = i2c_data_receive(I2C2);
        
        if(i == 5)
        {
            i2c_ack_enable(I2C2,FALSE);
            i2c_stop_generate(I2C2);
        }
    }
    
    //提取St[19:16]
    for(uint16_t i = 0;i<=3;i++)
    {
        if((frame_receive_buff[3] & (0x01 << i)) != 0)
        {
            temperature |= 0x01<<(16+i);
        }else{
            temperature &= ~(0x01<<(16+i));
        }
    }
    
    //提取St[15:8]
    for(uint16_t i = 0;i<=7;i++)
    {
        if((frame_receive_buff[4] & (0x01 << i)) != 0)
        {
            temperature |= 0x01<<(8+i);
        }else{
            temperature &= ~(0x01<<(8+i));
        }
    }
    
    //提取St[7:0]
    for(uint16_t i = 0;i<=7;i++)
    {
        if((frame_receive_buff[5] & (0x01 << i)) != 0)
        {
            temperature |= 0x01<<(0+i);
        }else{
            temperature &= ~(0x01<<(0+i));
        }
    }  
    
    //printf("Temperature: %d\n", temperature);
    result = (float)temperature/1048576*200-50;
    
    return result;
}

float aht20_get_humidity()
{
    uint32_t humidity = 0;
    float result = 0;
    
    uint8_t frame_receive_buff[7];
    
    //Transmit
    i2c_start_generate(I2C2);
    while(i2c_flag_get(I2C2,I2C_STARTF_FLAG)!= SET);
    
    i2c_7bit_address_send(I2C2,AHT20_IIC_ADDR,I2C_DIRECTION_TRANSMIT);
    while(i2c_flag_get(I2C2,I2C_ADDR7F_FLAG)!= SET);
    i2c_flag_clear(I2C2,I2C_ADDR7F_FLAG);
    
    i2c_data_send(I2C2,0xAC);
    while(i2c_flag_get(I2C2,I2C_TDBE_FLAG)!= SET);
    
    i2c_data_send(I2C2,0x33);
    while(i2c_flag_get(I2C2,I2C_TDBE_FLAG)!= SET);

    i2c_data_send(I2C2,0x00);
    while(i2c_flag_get(I2C2,I2C_TDBE_FLAG)!= SET);
    
    while(i2c_flag_get(I2C2,I2C_TDC_FLAG)!= SET);
    
    i2c_stop_generate(I2C2);
    
    //Receive
    i2c_ack_enable(I2C2,TRUE);
    i2c_start_generate(I2C2);
    while(i2c_flag_get(I2C2,I2C_STARTF_FLAG)!= SET);
    
    i2c_7bit_address_send(I2C2,AHT20_IIC_ADDR,I2C_DIRECTION_RECEIVE);
    while(i2c_flag_get(I2C2,I2C_ADDR7F_FLAG)!= SET);
    i2c_flag_clear(I2C2,I2C_ADDR7F_FLAG);
    
    for(uint16_t i = 0;i<7;i++)
    {
        while(i2c_flag_get(I2C2,I2C_RDBF_FLAG)!= SET);
        frame_receive_buff[i] = i2c_data_receive(I2C2);
        
        if(i == 5)
        {
            i2c_ack_enable(I2C2,FALSE);
            i2c_stop_generate(I2C2);
        }
    }
    
    //提取Srh[19:12]
    for(uint16_t i = 0;i<=7;i++)
    {
        if((frame_receive_buff[1] & (0x01 << i)) != 0)
        {
            humidity |= 0x01<<(12+i);
        }else{
            humidity &= ~(0x01<<(12+i));
        }
    }
    
    //提取St[11:4]
    for(uint16_t i = 0;i<=7;i++)
    {
        if((frame_receive_buff[2] & (0x01 << i)) != 0)
        {
            humidity |= 0x01<<(4+i);
        }else{
            humidity &= ~(0x01<<(4+i));
        }
    }
    
    //提取St[3:0]
    for(uint16_t i = 0;i<=3;i++)
    {
        if((frame_receive_buff[3] & (0x01 << i)) != 0)
        {
            humidity |= 0x01<<(0+i);
        }else{
            humidity &= ~(0x01<<(0+i));
        }
    }  
    
    //printf("humidity: %d\n", humidity);
    result = (float)humidity/1048576;
    
    return result;
}
*/
