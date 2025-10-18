#include "oled_ssd1306.h"

extern lv_display_t * disp_ssd1306;

uint8_t ssd1306_buffer[SSD1306_WIDTH * SSD1306_HEIGHT / 8] = {0};
uint8_t ssd1306_buffer_dma[SSD1306_WIDTH * SSD1306_HEIGHT / 8] = {0};
uint8_t dma1_channe2_FDT_is_finish = false;

void ssd1306_init(void);

void oled_ssd1306_init()
{
    gpio_init_type gpio_init_struct;
    dma_init_type dma_init_struct;

    //使能时钟
    crm_periph_clock_enable(CRM_IOMUX_PERIPH_CLOCK, TRUE);
    crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK, TRUE);
    crm_periph_clock_enable(CRM_I2C1_PERIPH_CLOCK, TRUE);
    crm_periph_clock_enable(CRM_DMA1_PERIPH_CLOCK, TRUE);
	
    //配置GPIO
    gpio_default_para_init(&gpio_init_struct);
    gpio_init_struct.gpio_out_type = GPIO_OUTPUT_OPEN_DRAIN;
    gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
    gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
    gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_MODERATE;
    gpio_init_struct.gpio_pins = GPIO_PINS_8; // SCL
    gpio_init(GPIOB, &gpio_init_struct);
    gpio_init_struct.gpio_out_type = GPIO_OUTPUT_OPEN_DRAIN;
    gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
    gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
    gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_MODERATE;
    gpio_init_struct.gpio_pins = GPIO_PINS_9; // SDA
    gpio_init(GPIOB, &gpio_init_struct);
    gpio_pin_remap_config(I2C1_GMUX_0001, TRUE);

    //配置DMA
    dma_reset(DMA1_CHANNEL2);
    dma_default_para_init(&dma_init_struct);
    dma_init_struct.direction = DMA_DIR_MEMORY_TO_PERIPHERAL;
    dma_init_struct.memory_data_width = DMA_MEMORY_DATA_WIDTH_BYTE;
    dma_init_struct.memory_inc_enable = TRUE;
    dma_init_struct.peripheral_data_width = DMA_PERIPHERAL_DATA_WIDTH_BYTE;
    dma_init_struct.peripheral_inc_enable = FALSE;
    dma_init_struct.priority = DMA_PRIORITY_LOW;
    dma_init_struct.loop_mode_enable = FALSE;
    dma_init(DMA1_CHANNEL2, &dma_init_struct);
    dma_flexible_config(DMA1, FLEX_CHANNEL2, DMA_FLEXIBLE_I2C1_TX);

    //配置I2C
    i2c_init(I2C1, I2C_FSMODE_DUTY_2_1, 400000);
    i2c_own_address1_set(I2C1, I2C_ADDRESS_MODE_7BIT, 0x0);
    i2c_ack_enable(I2C1, TRUE);
    i2c_clock_stretch_enable(I2C1, TRUE);
    i2c_general_call_enable(I2C1, FALSE);
    i2c_dma_enable(I2C1, TRUE);
    i2c_enable(I2C1, TRUE);

    //配置中断
    nvic_irq_enable(DMA1_Channel2_IRQn, 9, 0);
    nvic_irq_enable(I2C1_EVT_IRQn, 10, 0);
    dma_interrupt_enable(DMA1_CHANNEL2, DMA_FDT_INT, TRUE);
    
    //初始化SSD1306
    ssd1306_init();
}

/* ==================== IIC数据传输 ================ */

static void send_cmd(uint8_t code) {
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

// SSD1306配置为页面寻址模式时可用
// void ssd1306_flush(void) {
//     // Send the display buffer to the SSD1306
//     for (uint8_t page = 0; page < 8; page++) {
//         send_cmd(0xB0 + page); // Set page address
//         send_cmd(0x00); // Set lower column address
//         send_cmd(0x10); // Set higher column address

//         for (uint16_t col = 0; col < SSD1306_WIDTH; col++) {
//             send_data(ssd1306_buffer[col + (page * SSD1306_WIDTH)]);
//         }
//     }
// }

void ssd1306_flush_dma(void) 
{
    dma_channel_enable(DMA1_CHANNEL2, FALSE);

    DMA1_CHANNEL2->dtcnt = sizeof(ssd1306_buffer_dma);
    DMA1_CHANNEL2->paddr = (uint32_t)&(I2C1->dt);
    DMA1_CHANNEL2->maddr = (uint32_t)ssd1306_buffer_dma;

    i2c_start_generate(I2C1);
    while(i2c_flag_get(I2C1,I2C_STARTF_FLAG)!= SET);
    
    i2c_7bit_address_send(I2C1,SSD1306_IIC_ADDR,I2C_DIRECTION_TRANSMIT);
    while(i2c_flag_get(I2C1,I2C_ADDR7F_FLAG)!= SET);
    i2c_flag_clear(I2C1,I2C_ADDR7F_FLAG);

    i2c_data_send(I2C1,0x40);

    dma_channel_enable(DMA1_CHANNEL2, TRUE);
}

void DMA1_Channel2_IRQHandler(void)
{
    if(dma_flag_get(DMA1_FDT2_FLAG)== SET) 
    {
        dma_flag_clear(DMA1_FDT2_FLAG);
        dma1_channe2_FDT_is_finish = true;
        i2c_interrupt_enable(I2C1, I2C_EVT_INT, TRUE);
    }
}

void I2C1_EVT_IRQHandler(void)
{
    if(i2c_flag_get(I2C1,I2C_TDC_FLAG)== SET && dma1_channe2_FDT_is_finish == true) 
    {
        i2c_interrupt_enable(I2C1, I2C_EVT_INT, FALSE);

        i2c_stop_generate(I2C1);
					
        dma1_channe2_FDT_is_finish = false;
				
        lv_display_flush_ready(disp_ssd1306);   
    }
}

/* ==================== SSD1306初始化 ==================== */

void ssd1306_init(void) {
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
    //send_cmd(0x02); // Page addressing mode
    send_cmd(0x01); // Vertical addressing mode
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
    ssd1306_flush_dma();
}

/* ==================== 对ssd1306_buffer的操作 ==================== */

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




