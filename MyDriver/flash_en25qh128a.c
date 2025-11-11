#include "flash_en25qh128a.h"

void flash_en25qh_init()
{
	gpio_init_type gpio_init_struct;
    spi_init_type spi_init_struct;

	crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);
	crm_periph_clock_enable(CRM_GPIOC_PERIPH_CLOCK, TRUE);
	crm_periph_clock_enable(CRM_SPI1_PERIPH_CLOCK, TRUE);
	
    gpio_default_para_init(&gpio_init_struct);
    spi_default_para_init(&spi_init_struct);

	gpio_bits_set(GPIOA, GPIO_PINS_4);
	gpio_bits_set(GPIOC, GPIO_PINS_4 | GPIO_PINS_5);
	
	gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_MODERATE;
    gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
    gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;
    gpio_init_struct.gpio_pins = GPIO_PINS_4;
    gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
    gpio_init(GPIOA, &gpio_init_struct);
	
	gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_MODERATE;
    gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
    gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;
    gpio_init_struct.gpio_pins = GPIO_PINS_4 | GPIO_PINS_5;
    gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
    gpio_init(GPIOC, &gpio_init_struct);
	
    /* configure the SCK pin */
    gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_MODERATE;
    gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
    gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
    gpio_init_struct.gpio_pins = GPIO_PINS_5;
    gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
    gpio_init(GPIOA, &gpio_init_struct);

    /* configure the MISO pin */
    gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_MODERATE;
    gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
    gpio_init_struct.gpio_mode = GPIO_MODE_INPUT;
    gpio_init_struct.gpio_pins = GPIO_PINS_6;
    gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
    gpio_init(GPIOA, &gpio_init_struct);

    /* configure the MOSI pin */
    gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_MODERATE;
    gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
    gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
    gpio_init_struct.gpio_pins = GPIO_PINS_7;
    gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
    gpio_init(GPIOA, &gpio_init_struct);

    /* configure param */
    spi_init_struct.transmission_mode = SPI_TRANSMIT_FULL_DUPLEX;
    spi_init_struct.master_slave_mode = SPI_MODE_MASTER;
    spi_init_struct.frame_bit_num = SPI_FRAME_8BIT;
    spi_init_struct.first_bit_transmission = SPI_FIRST_BIT_MSB;
    spi_init_struct.mclk_freq_division = SPI_MCLK_DIV_32;
    spi_init_struct.clock_polarity = SPI_CLOCK_POLARITY_LOW;
    spi_init_struct.clock_phase = SPI_CLOCK_PHASE_1EDGE;
    spi_init_struct.cs_mode_selection = SPI_CS_SOFTWARE_MODE;
    spi_init(SPI1, &spi_init_struct);

    spi_enable(SPI1, TRUE);
}

/**
 * @brief  Read the JEDEC identification of the EN25QH128A flash memory.
 * @param  manufacturer: Pointer to store the manufacturer ID.
 * @param  memory_type: Pointer to store the memory type.
 * @param  memory_capacity: Pointer to store the memory capacity.
 * @retval true if the operation is successful, false otherwise.
 */
bool en25qh_read_identification(uint8_t *manufacturer, uint8_t *memory_type, uint8_t *memory_capacity)
{
    EN25QH_WP_H; // 禁用写保护
    EN25QH_HOLD_H; // 禁用写保护和保持功能
    EN25QH_CS_L; // 使能片选

    while(spi_i2s_flag_get(EN25QH_SPI, SPI_I2S_TDBE_FLAG) == RESET);
    spi_i2s_data_transmit(EN25QH_SPI, 0x9F);
    while(spi_i2s_flag_get(EN25QH_SPI, SPI_I2S_RDBF_FLAG) == RESET);
    spi_i2s_data_receive(EN25QH_SPI);   //dummy read to clear the flag
    while(spi_i2s_flag_get(EN25QH_SPI, SPI_I2S_BF_FLAG) == SET);

    // 接收制造商ID - 需要发送虚拟字节驱动时钟
    while(spi_i2s_flag_get(EN25QH_SPI, SPI_I2S_TDBE_FLAG) == RESET);
    spi_i2s_data_transmit(EN25QH_SPI, EN25QH_SPI_DUMMY_BYTE);  // 驱动时钟的虚拟发送
    while(spi_i2s_flag_get(EN25QH_SPI, SPI_I2S_RDBF_FLAG) == RESET);
    *manufacturer = (uint8_t)spi_i2s_data_receive(EN25QH_SPI);
    while(spi_i2s_flag_get(EN25QH_SPI, SPI_I2S_BF_FLAG) == SET);

    // 接收内存类型 - 同样需要虚拟发送
    while(spi_i2s_flag_get(EN25QH_SPI, SPI_I2S_TDBE_FLAG) == RESET);
    spi_i2s_data_transmit(EN25QH_SPI, EN25QH_SPI_DUMMY_BYTE);  // 驱动时钟
    while(spi_i2s_flag_get(EN25QH_SPI, SPI_I2S_RDBF_FLAG) == RESET);
    *memory_type = (uint8_t)spi_i2s_data_receive(EN25QH_SPI);
    while(spi_i2s_flag_get(EN25QH_SPI, SPI_I2S_BF_FLAG) == SET);

    // 接收内存容量
    while(spi_i2s_flag_get(EN25QH_SPI, SPI_I2S_TDBE_FLAG) == RESET);
    spi_i2s_data_transmit(EN25QH_SPI, EN25QH_SPI_DUMMY_BYTE);  // 驱动时钟
    while(spi_i2s_flag_get(EN25QH_SPI, SPI_I2S_RDBF_FLAG) == RESET);
    *memory_capacity = (uint8_t)spi_i2s_data_receive(EN25QH_SPI);
    while(spi_i2s_flag_get(EN25QH_SPI, SPI_I2S_BF_FLAG) == SET);

    EN25QH_CS_H; // 禁用片选

    return true;
}

bool en25qh_wait_busy(void)
{
    uint8_t status;

    EN25QH_WP_H; 
    EN25QH_HOLD_H; 
    EN25QH_CS_L;

    while(spi_i2s_flag_get(EN25QH_SPI, SPI_I2S_TDBE_FLAG) == RESET);
    spi_i2s_data_transmit(EN25QH_SPI, 0x05);    // 发送读取状态寄存器命令
    while(spi_i2s_flag_get(EN25QH_SPI, SPI_I2S_RDBF_FLAG) == RESET);
    spi_i2s_data_receive(EN25QH_SPI);   //dummy read to clear the flag
    while(spi_i2s_flag_get(EN25QH_SPI, SPI_I2S_BF_FLAG) == SET);

    do{
        while(spi_i2s_flag_get(EN25QH_SPI, SPI_I2S_TDBE_FLAG) == RESET);
        spi_i2s_data_transmit(EN25QH_SPI, EN25QH_SPI_DUMMY_BYTE); // 发送虚拟字节以驱动时钟
        while(spi_i2s_flag_get(EN25QH_SPI, SPI_I2S_RDBF_FLAG) == RESET);
        status = (uint8_t)spi_i2s_data_receive(EN25QH_SPI);
        while(spi_i2s_flag_get(EN25QH_SPI, SPI_I2S_BF_FLAG) == SET);
    } while((status & 0x01) == 0x01); // 检查忙状态

    EN25QH_CS_H;

    return true;
}

bool en25qh_write_enable(void)
{
    EN25QH_WP_H; // 禁用写保护
    EN25QH_HOLD_H; // 禁用保持功能
    EN25QH_CS_L; // 使能片选

    while(spi_i2s_flag_get(EN25QH_SPI, SPI_I2S_TDBE_FLAG) == RESET);
    spi_i2s_data_transmit(EN25QH_SPI, 0x06); // 发送写使能命令
    while(spi_i2s_flag_get(EN25QH_SPI, SPI_I2S_RDBF_FLAG) == RESET);
    spi_i2s_data_receive(EN25QH_SPI);   //dummy read to clear the flag
    while(spi_i2s_flag_get(EN25QH_SPI, SPI_I2S_BF_FLAG) == SET);

    EN25QH_CS_H; // 失能片选

    return true;
}

/**
 * @brief  Erase a sector in the EN25QH128A flash memory.
 * @param  addr: The address of the sector to erase.
 * @retval true if the operation is successful, false otherwise.
 */
bool en25qh_erase_sector(uint32_t addr)
{
    en25qh_write_enable(); // 使能写操作

    EN25QH_WP_H; // 禁用写保护
    EN25QH_HOLD_H; // 禁用保持功能
    EN25QH_CS_L; // 使能片选

    while(spi_i2s_flag_get(EN25QH_SPI, SPI_I2S_TDBE_FLAG) == RESET);
    spi_i2s_data_transmit(EN25QH_SPI, 0x20); // 发送扇区擦除命令
    while(spi_i2s_flag_get(EN25QH_SPI, SPI_I2S_RDBF_FLAG) == RESET);
    spi_i2s_data_receive(EN25QH_SPI);   //dummy read to clear the flag
    while(spi_i2s_flag_get(EN25QH_SPI, SPI_I2S_BF_FLAG) == SET);

    // 发送地址
    for (int i = 0; i < 3; i++) {
        while(spi_i2s_flag_get(EN25QH_SPI, SPI_I2S_TDBE_FLAG) == RESET);
        spi_i2s_data_transmit(EN25QH_SPI, (addr >> (16 - i * 8)) & 0xFF);
        while(spi_i2s_flag_get(EN25QH_SPI, SPI_I2S_RDBF_FLAG) == RESET);
        spi_i2s_data_receive(EN25QH_SPI);   //dummy read to clear the flag
        while(spi_i2s_flag_get(EN25QH_SPI, SPI_I2S_BF_FLAG) == SET);
    }

    EN25QH_CS_H; // 失能片选

    en25qh_wait_busy(); // 等待擦除完成

    return true;
}

bool en25qh_write_page(uint32_t addr, const uint8_t *buf, uint16_t len)
{
    if (len > 256) {
        return false; // 页面写入长度不能超过256字节
    }

    en25qh_write_enable(); // 使能写操作

    EN25QH_WP_H; // 禁用写保护
    EN25QH_HOLD_H; // 禁用保持功能
    EN25QH_CS_L; // 使能片选

    while(spi_i2s_flag_get(EN25QH_SPI, SPI_I2S_TDBE_FLAG) == RESET);
    spi_i2s_data_transmit(EN25QH_SPI, 0x02); // 发送页面编程命令
    while(spi_i2s_flag_get(EN25QH_SPI, SPI_I2S_RDBF_FLAG) == RESET);
    spi_i2s_data_receive(EN25QH_SPI);   //dummy read to clear the flag
    while(spi_i2s_flag_get(EN25QH_SPI, SPI_I2S_BF_FLAG) == SET);

    // 发送地址
    for (int i = 0; i < 3; i++) {
        while(spi_i2s_flag_get(EN25QH_SPI, SPI_I2S_TDBE_FLAG) == RESET);
        spi_i2s_data_transmit(EN25QH_SPI, (addr >> (16 - i * 8)) & 0xFF);
        while(spi_i2s_flag_get(EN25QH_SPI, SPI_I2S_RDBF_FLAG) == RESET);
        spi_i2s_data_receive(EN25QH_SPI);   //dummy read to clear the flag
        while(spi_i2s_flag_get(EN25QH_SPI, SPI_I2S_BF_FLAG) == SET);
    }

    // 写入数据
    for (uint16_t i = 0; i < len; i++) {
        while(spi_i2s_flag_get(EN25QH_SPI, SPI_I2S_TDBE_FLAG) == RESET);
        spi_i2s_data_transmit(EN25QH_SPI, buf[i]);
        while(spi_i2s_flag_get(EN25QH_SPI, SPI_I2S_RDBF_FLAG) == RESET);
        spi_i2s_data_receive(EN25QH_SPI);   //dummy read to clear the flag
        while(spi_i2s_flag_get(EN25QH_SPI, SPI_I2S_BF_FLAG) == SET);
    }

    EN25QH_CS_H; // 失能片选

    en25qh_wait_busy(); // 等待写入完成

    return true;
}

bool en25qh_erase_chip(void)
{
    en25qh_write_enable(); // 使能写操作

    EN25QH_WP_H; // 禁用写保护
    EN25QH_HOLD_H; // 禁用保持功能
    EN25QH_CS_L; // 使能片选

    while(spi_i2s_flag_get(EN25QH_SPI, SPI_I2S_TDBE_FLAG) == RESET);
    spi_i2s_data_transmit(EN25QH_SPI, 0xC7); // 发送芯片擦除命令
    while(spi_i2s_flag_get(EN25QH_SPI, SPI_I2S_RDBF_FLAG) == RESET);
    spi_i2s_data_receive(EN25QH_SPI);   //dummy read to clear the flag
    while(spi_i2s_flag_get(EN25QH_SPI, SPI_I2S_BF_FLAG) == SET);

    EN25QH_CS_H; // 失能片选

    en25qh_wait_busy(); // 等待擦除完成

    return true;
}

/**
 * @brief  Read data from the EN25QH128A flash memory.
 * @param  addr: The starting address to read from.
 * @param  buf: Pointer to the buffer to store the read data.
 * @param  len: Number of bytes to read.
 * @retval true if the operation is successful, false otherwise.
 */
bool en25qh_read_data(uint32_t addr, uint8_t *buf, uint32_t len)
{
    EN25QH_WP_H; // 禁用写保护
    EN25QH_HOLD_H; // 禁用保持功能
    EN25QH_CS_L; // 使能片选

    while(spi_i2s_flag_get(EN25QH_SPI, SPI_I2S_TDBE_FLAG) == RESET);
    spi_i2s_data_transmit(EN25QH_SPI, 0x03); // 发送读取数据命令
    while(spi_i2s_flag_get(EN25QH_SPI, SPI_I2S_RDBF_FLAG) == RESET);
    spi_i2s_data_receive(EN25QH_SPI);   //dummy read to clear the flag
    while(spi_i2s_flag_get(EN25QH_SPI, SPI_I2S_BF_FLAG) == SET);

    // 发送地址
    for (int i = 0; i < 3; i++) {
        while(spi_i2s_flag_get(EN25QH_SPI, SPI_I2S_TDBE_FLAG) == RESET);
        spi_i2s_data_transmit(EN25QH_SPI, (addr >> (16 - i * 8)) & 0xFF);
        while(spi_i2s_flag_get(EN25QH_SPI, SPI_I2S_RDBF_FLAG) == RESET);
        spi_i2s_data_receive(EN25QH_SPI);   //dummy read to clear the flag
        while(spi_i2s_flag_get(EN25QH_SPI, SPI_I2S_BF_FLAG) == SET);
    }

    // 读取数据
    for (uint32_t i = 0; i < len; i++) {
        while(spi_i2s_flag_get(EN25QH_SPI, SPI_I2S_TDBE_FLAG) == RESET);
        spi_i2s_data_transmit(EN25QH_SPI, EN25QH_SPI_DUMMY_BYTE); // 发送虚拟字节以驱动时钟
        while(spi_i2s_flag_get(EN25QH_SPI, SPI_I2S_RDBF_FLAG) == RESET);
        buf[i] = (uint8_t)spi_i2s_data_receive(EN25QH_SPI);
        while(spi_i2s_flag_get(EN25QH_SPI, SPI_I2S_BF_FLAG) == SET);
    }

    EN25QH_CS_H; // 失能片选

    return true;
}

/********************************************************************************************/
/** @brief  Write data to the EN25QH128A flash memory.
 * @param  buf: Pointer to the buffer containing the data to write.
 * @param  sector: The starting sector number to write to.
 * @param  count: Number of sectors to write.
 * @retval true if the operation is successful, false otherwise.
 */
bool en25qh_write_data(uint8_t *buf, uint32_t sector, uint32_t count) {
    uint32_t sector_erase_addr = sector * EN25QH_SECTOR_SIZE;
    uint32_t page_write_addr = sector * EN25QH_SECTOR_SIZE;
    uint8_t *buf_p = buf;

    for(uint32_t i = 1; i <= count; i++) {                  // 进行count次扇区擦写
        if(!en25qh_erase_sector(sector_erase_addr)) {       
            return false;                                   // 擦除失败
        }

        for(uint32_t j = 1; j <= EN25QH_PAGE_PER_SECTOR; j++) {//每个扇区需要EN25QH_PAGE_PER_SECTOR次页写
            if(!en25qh_write_page(page_write_addr, buf_p, EN25QH_PAGE_SIZE)) {
                return false;                               // 页面写入失败
            }

            buf_p += EN25QH_PAGE_SIZE;                      // 更新缓冲区指针
            page_write_addr += EN25QH_PAGE_SIZE;            // 更新写入地址
        }

        buf_p += EN25QH_SECTOR_SIZE;                        // 更新缓冲区指针
        sector_erase_addr +=  EN25QH_SECTOR_SIZE;           // 更新擦除地址
        page_write_addr +=  EN25QH_SECTOR_SIZE;             // 更新写入地址
    }

    return true;
}

/** @brief  Write data to the EN25QH128A flash memory with read-modify-write for unaligned writes.
 * @param  addr: The starting address to write to.
 * @param  buf: Pointer to the buffer containing the data to write.
 * @param  len: write length
 * @retval true if the operation is successful, false otherwise.
 */
 bool en25qh_write_data_2(uint32_t addr, uint8_t *buf, uint32_t len)
 {
    uint32_t sector_size = EN25QH_SECTOR_SIZE;
    uint32_t page_size = EN25QH_PAGE_SIZE;
    uint32_t current_addr = addr;
    uint32_t bytes_remaining = len;
    uint8_t *current_buf = buf;

    // 静态缓冲区用于读-修改-写操作（避免栈溢出）
    static uint8_t sector_buffer[EN25QH_SECTOR_SIZE];
    
    if (sector_size > sizeof(sector_buffer)) {
        return false; // 缓冲区太小
    }

    while (bytes_remaining > 0) {
        // 计算当前地址所在的扇区
        uint32_t sector_start = current_addr - (current_addr % sector_size);
        uint32_t sector_offset = current_addr - sector_start;
        
        // 计算本次写入在扇区内的长度
        uint32_t bytes_this_sector = sector_size - sector_offset;
        if (bytes_this_sector > bytes_remaining) {
            bytes_this_sector = bytes_remaining;
        }

        // 方法1：如果要写入整个扇区，直接擦除并写入（最快）
        if (sector_offset == 0 && bytes_this_sector == sector_size) {
            if (!en25qh_erase_sector(sector_start)) {
                return false;
            }
            
            // 按页写入整个扇区
            for (uint32_t i = 0; i < sector_size; i += page_size) {
                uint32_t write_size = (sector_size - i) < page_size ? (sector_size - i) : page_size;
                if (!en25qh_write_page(sector_start + i, current_buf + i, write_size)) {
                    return false;
                }
            }
        }
        // 方法2：部分扇区写入，需要读-修改-写操作
        else {
            // 1. 读取整个扇区到缓冲区
            if (!en25qh_read_data(sector_start, sector_buffer, sector_size)) {
                return false;
            }
            
            // 2. 将新数据复制到缓冲区的相应位置
            memcpy(sector_buffer + sector_offset, current_buf, bytes_this_sector);
            
            // 3. 擦除整个扇区
            if (!en25qh_erase_sector(sector_start)) {
                return false;
            }
            
            // 4. 将修改后的缓冲区写回扇区
            for (uint32_t i = 0; i < sector_size; i += page_size) {
                uint32_t write_size = (sector_size - i) < page_size ? (sector_size - i) : page_size;
                if (!en25qh_write_page(sector_start + i, sector_buffer + i, write_size)) {
                    return false;
                }
            }
        }

        // 更新指针和剩余字节数
        current_addr += bytes_this_sector;
        current_buf += bytes_this_sector;
        bytes_remaining -= bytes_this_sector;
    }

    return true;
 }
 
