#include "RTE_Components.h"
#include  CMSIS_device_header

#include "cmsis_os2.h"

#include "system.h"
#include "delay.h"
#include "calendar_rtc.h"
#include "led.h"
#include "button.h"
#include "flash_en25qh128a.h"
#include "oled_ssd1306.h"
#include "fan_ss6285l.h"
#include "sensor_aht20.h"
#include "encoder_ec11.h"
#include "file_system.h"
#include "usbdevice.h"
#include "voltage_adc.h"

#include "lvgl.h"
#include "lv_port_disp_template.h"
#include "lv_port_indev_template.h"
#include "my_gui.h"


/******************* extern func *****************************/
void app_main (void *argument);
void app_lvgl(void *argument);


/******************* main func ****************************/

int main (void) 
{
	system_init();
	delay_init();
	calendar_rtc_init();
	fan_ss6285l_init(); 
	oled_ssd1306_init();
	encoder_ec11_init();
	flash_en25qh_init();
	sensor_aht20_init();
	led_init();
	at32_button_init();
	usbdevice_init();
	voltage_adc_init();
	
	const osThreadAttr_t thread_main_attr = {
		.stack_size = 13000                           // Create the thread stack with a size of 16384 bytes
	};
	
  osKernelInitialize();                 // Initialize CMSIS-RTOS
  osThreadNew(app_main, NULL, &thread_main_attr);    // Create application main thread
  osKernelStart();                      // Start thread execution

  while(1);
}

/************************** app func ****************************/
void app_main (void *argument)
{
	file_system_init();
	
	const osThreadAttr_t thread_lvgl_attr = {
		.stack_size = 20000                           // Create the thread stack with a size of 16384 bytes
	};
	osThreadNew(app_lvgl, NULL, &thread_lvgl_attr);
	
  while(1)
	{
		osDelay(osWaitForever);
	}
}

void app_lvgl(void *argument)
{
	lv_init();
	lv_tick_set_cb(osKernelGetTickCount);  //滴答中断频率设为1000, tick数就是ms数
	lv_port_disp_init();
	lv_port_indev_init();
	lvgl_ui();

  while(1)
	{
		uint32_t time_till_next = lv_timer_handler();
  	if(time_till_next == LV_NO_TIMER_READY) time_till_next = LV_DEF_REFR_PERIOD; /*handle LV_NO_TIMER_READY. Another option is to `sleep` for longer*/
  	osDelay(time_till_next); /* delay to avoid unnecessary polling */
	}
}
