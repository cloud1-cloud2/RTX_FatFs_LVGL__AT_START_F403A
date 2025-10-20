#ifndef FLASH_EN25QH128A_H
#define FLASH_EN25QH128A_H

#include "RTE_Components.h"
#include CMSIS_device_header

#include "stdint.h"
#include <stdbool.h>
#include <stddef.h> // For NULL definition

#define EN25QH_SPI                    SPI1
#define EN25QH_SPI_DUMMY_BYTE         0xA5

#define EN25QH_CS_PORT GPIOA
#define EN25QH_CS_PIN GPIO_PINS_4
#define EN25QH_WP_PORT GPIOC
#define EN25QH_WP_PIN GPIO_PINS_4
#define EN25QH_HOLD_PORT GPIOC
#define EN25QH_HOLD_PIN GPIO_PINS_5

#define EN25QH_CS_H gpio_bits_set(EN25QH_CS_PORT,EN25QH_CS_PIN)
#define EN25QH_CS_L gpio_bits_reset(EN25QH_CS_PORT,EN25QH_CS_PIN)
#define EN25QH_WP_H gpio_bits_set(EN25QH_WP_PORT,EN25QH_WP_PIN)
#define EN25QH_WP_L gpio_bits_reset(EN25QH_WP_PORT,EN25QH_WP_PIN)
#define EN25QH_HOLD_H gpio_bits_set(EN25QH_HOLD_PORT,EN25QH_HOLD_PIN)
#define EN25QH_HOLD_L gpio_bits_reset(EN25QH_HOLD_PORT,EN25QH_HOLD_PIN)

#define EN25QH_SECTOR_SIZE 4096 // 扇区大小为4K字节
#define EN25QH_SECTOR_COUNT 4096 // 总扇区数为4096
#define EN25QH_SECTOR_PER_BLOCK 8 // 每个块包含8个扇区
#define EN25QH_PAGE_SIZE 256 // 页面大小为256字节
#define EN25QH_PAGE_PER_SECTOR 16 // 每个扇区包含16个页面

void flash_en25qh_init(void);
bool en25qh_read_identification(uint8_t *manufacturer, uint8_t *memory_type, uint8_t *memory_capacity);
bool en25qh_write_enable(void);
bool en25qh_erase_chip(void);
bool en25qh_erase_sector(uint32_t addr);
bool en25qh_write_page(uint32_t addr, const uint8_t *buf, uint16_t len);
bool en25qh_read_data(uint32_t addr, uint8_t *buf, uint32_t len);
bool en25qh_wait_busy(void);
bool en25qh_write_data(uint8_t *buf, uint32_t sector, uint32_t count);

#endif
