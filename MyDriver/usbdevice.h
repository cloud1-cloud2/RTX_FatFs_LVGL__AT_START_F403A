#ifndef USBDEVICE_H
#define USBDEVICE_H

#include "RTE_Components.h"
#include  CMSIS_device_header

#include "delay.h"
#include "usbd_core.h"
#include "custom_hid_class.h"
#include "custom_hid_desc.h"
#include "usbd_int.h"

#pragma pack(push, 1)
typedef struct {
	uint8_t  report_id;     // 为0x01
	
	// 左摇杆 (X/Y, 各16位)
	uint16_t left_x;
	uint16_t left_y;
	
	// 右摇杆 (Z/Rz, 各16位)
	uint16_t right_z;
	uint16_t right_rz;
	
	// 左扳机 (10位 + 6填充位)
	uint16_t left_trigger;  // Bit 0-9: 模拟值 (0-1023), Bit 10-15: 填充0
	
	// 右扳机 (10位 + 6填充位)
	uint16_t right_trigger; // Bit 0-9: 模拟值 (0-1023), Bit 10-15: 填充0
	
	// 方向键 (4位) + 填充 (4位)
	uint8_t  dpad;          // Bit 0-3: 方向值, Bit 4-7: 填充0
	
	// 15个按钮 (Bit 0-14) + 1填充位
	uint16_t buttons;       // Bit 0: A, Bit 1: B, ..., Bit 14: RSB
	
	// 分享按钮 (1位) + 7填充位
	uint8_t  share_button;  // Bit 0: 分享键, Bit 1-7: 填充0
} gamepad_report1_t;

typedef struct {
	uint8_t report_id;      // 必须为0x02 (1字节)
	uint8_t enable;         // 电机使能 (低4位有效) (1字节)
	uint8_t magnitude[4];   // 4个电机强度 0-100% (4字节)
	uint8_t duration;       // 持续时间 (单位10ms) (1字节)  
	uint8_t start_delay;    // 启动延迟 (单位10ms) (1字节)
	uint8_t loop_count;     // 循环次数 (1字节)
} gamepad_report2_t;
#pragma pack(pop)

extern usbd_core_type usb_core_dev;
extern gamepad_report1_t report1;
extern gamepad_report2_t report2;

void usb_clock48m_select(usb_clk48_s clk_s);
void usbdevice_init();

#endif
