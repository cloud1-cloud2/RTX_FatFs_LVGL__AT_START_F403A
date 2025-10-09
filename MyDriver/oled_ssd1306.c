#include "oled_ssd1306.h"
#include "stdlib.h"

uint8_t ssd1306_buffer[SSD1306_WIDTH * SSD1306_HEIGHT / 8] = {0};

void ssd1306_init(void);

void oled_ssd1306_init()
{
	gpio_init_type gpio_init_struct;

	crm_periph_clock_enable(CRM_IOMUX_PERIPH_CLOCK, TRUE);
	crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK, TRUE);
	crm_periph_clock_enable(CRM_I2C1_PERIPH_CLOCK, TRUE);
	
  gpio_default_para_init(&gpio_init_struct);

  /* configure the SCL pin */
  gpio_init_struct.gpio_out_type = GPIO_OUTPUT_OPEN_DRAIN;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_MODERATE;
  gpio_init_struct.gpio_pins = GPIO_PINS_8;
  gpio_init(GPIOB, &gpio_init_struct);

  /* configure the SDA pin */
  gpio_init_struct.gpio_out_type = GPIO_OUTPUT_OPEN_DRAIN;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_MODERATE;
  gpio_init_struct.gpio_pins = GPIO_PINS_9;
  gpio_init(GPIOB, &gpio_init_struct);

  gpio_pin_remap_config(I2C1_GMUX_0001, TRUE);

  i2c_init(I2C1, I2C_FSMODE_DUTY_2_1, 400000);
  i2c_own_address1_set(I2C1, I2C_ADDRESS_MODE_7BIT, 0x0);
  i2c_ack_enable(I2C1, TRUE);
  i2c_clock_stretch_enable(I2C1, TRUE);
  i2c_general_call_enable(I2C1, FALSE);

  i2c_enable(I2C1, TRUE);

  ssd1306_init();
}

static void send_cmd(uint8_t code) {
    // Function to send command to SSD1306
    // This function should be implemented based on your specific hardware interface
    
    i2c_start_generate(I2C1);
    while(i2c_flag_get(I2C1,I2C_STARTF_FLAG)!= SET);
    
    i2c_7bit_address_send(I2C1,SSD1306_IIC_ADDR,I2C_DIRECTION_TRANSMIT);
    while(i2c_flag_get(I2C1,I2C_ADDR7F_FLAG)!= SET);
    i2c_flag_clear(I2C1,I2C_ADDR7F_FLAG);
    
    i2c_data_send(I2C1,0x00);
    i2c_data_send(I2C1,code);
    while(i2c_flag_get(I2C1,I2C_TDC_FLAG)!= SET);
    
    i2c_stop_generate(I2C1);
     
}

static void send_data(uint8_t data) {
    // Function to send data to SSD1306
    // This function should be implemented based on your specific hardware interface
    
    i2c_start_generate(I2C1);
    while(i2c_flag_get(I2C1,I2C_STARTF_FLAG)!= SET);
    
    i2c_7bit_address_send(I2C1,SSD1306_IIC_ADDR,I2C_DIRECTION_TRANSMIT);
    while(i2c_flag_get(I2C1,I2C_ADDR7F_FLAG)!= SET);
    i2c_flag_clear(I2C1,I2C_ADDR7F_FLAG);
    
    i2c_data_send(I2C1,0x40);
    i2c_data_send(I2C1,data);
    while(i2c_flag_get(I2C1,I2C_TDC_FLAG)!= SET);
    
    i2c_stop_generate(I2C1);

}

void ssd1306_flush(void) {
    // Send the display buffer to the SSD1306
    for (uint8_t page = 0; page < 8; page++) {
        send_cmd(0xB0 + page); // Set page address
        send_cmd(0x00); // Set lower column address
        send_cmd(0x10); // Set higher column address

        for (uint16_t col = 0; col < SSD1306_WIDTH; col++) {
            send_data(ssd1306_buffer[col + (page * SSD1306_WIDTH)]);
        }
    }
}


void ssd1306_clear(void) {
    // Clear the display buffer
    for (uint16_t i = 0; i < sizeof(ssd1306_buffer); i++) {
        ssd1306_buffer[i] = 0x00;
    }
}

void ssd1306_draw_pixel(uint8_t x, uint8_t y, uint8_t color) {
    if (x >= SSD1306_WIDTH || y >= SSD1306_HEIGHT) {
        return; // Out of bounds
    }
    
    if (color) {
        ssd1306_buffer[x + (y / 8) * SSD1306_WIDTH] |= (1 << (y % 8));
    } else {
        ssd1306_buffer[x + (y / 8) * SSD1306_WIDTH] &= ~(1 << (y % 8));
    }
}

void ssd1306_draw_line(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t color) {
    // Bresenham's line algorithm
    int16_t dx = abs(x1 - x0);
    int16_t dy = abs(y1 - y0);
    int16_t sx = (x0 < x1) ? 1 : -1;
    int16_t sy = (y0 < y1) ? 1 : -1;
    int16_t err = dx - dy;

    while (1) {
        ssd1306_draw_pixel(x0, y0, color);
        if (x0 == x1 && y0 == y1) break;
        int16_t err2 = err * 2;
        if (err2 > -dy) {
            err -= dy;
            x0 += sx;
        }
        if (err2 < dx) {
            err += dx;
            y0 += sy;
        }
    }
}



void ssd1306_init(void) {
    // Initialize the SSD1306 display
    send_cmd(0xAE); // Display off
    send_cmd(0xD5); // Set display clock divide ratio/oscillator frequency
    send_cmd(0x80); // Set divide ratio
    send_cmd(0xA8); // Set multiplex ratio
    send_cmd(0x3F); // 1/64 duty
    send_cmd(0xD3); // Set display offset
    send_cmd(0x00); // No offset
    send_cmd(0x40); // Set start line to 0
    send_cmd(0x8D); // Charge pump setting
    send_cmd(0x14); // Enable charge pump
    send_cmd(0x20); // Memory addressing mode
    send_cmd(0x02); // Page addressing mode
    send_cmd(0xA1); // Segment remap (column address 127 is mapped to SEG0)
    send_cmd(0xC8); // COM output scan direction (remap mode)
    send_cmd(0xDA); // Set COM pins hardware configuration
    send_cmd(0x12);
    send_cmd(0x81); // Contrast control
    send_cmd(0xCF);
    send_cmd(0xD9); // Pre-charge period
    send_cmd(0xF1);
    send_cmd(0xDB); // VCOMH deselect level
    send_cmd(0x30);
    send_cmd(0xA4); // Entire display ON (resume to RAM content display)
    send_cmd(0xA6); // Normal display (not inverted)
    send_cmd(0xAF); // Display ON

    ssd1306_clear();
    ssd1306_flush();
}



