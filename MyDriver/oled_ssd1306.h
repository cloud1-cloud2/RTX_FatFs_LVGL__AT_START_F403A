#ifndef OLED_SSD1306_H
#define OLED_SSD1306_H

#include "RTE_Components.h"
#include  CMSIS_device_header
#include "stdlib.h"
#include "lvgl.h"
#include "stdbool.h"

#define SSD1306_WIDTH 128
#define SSD1306_HEIGHT 64
#define SSD1306_IIC_ADDR 0x78

extern uint8_t ssd1306_buffer[SSD1306_WIDTH * SSD1306_HEIGHT / 8];
extern uint8_t ssd1306_buffer_dma[SSD1306_WIDTH * SSD1306_HEIGHT / 8];

extern void oled_ssd1306_init(void);
//extern void ssd1306_flush(void);
extern void ssd1306_flush_dma(void);
extern void ssd1306_draw_pixel(uint8_t x, uint8_t y, uint8_t color);
extern void ssd1306_clear(void);
extern void ssd1306_draw_line(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t color);

#endif
