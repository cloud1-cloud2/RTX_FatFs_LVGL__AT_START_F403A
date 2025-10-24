/**
 * @file lv_port_disp_templ.c
 *
 */

/*set this value to "1" to enable content*/

#if 1

#include "lv_port_disp.h"

#ifndef MY_DISP_HOR_RES
    #define MY_DISP_HOR_RES    128
#endif

#ifndef MY_DISP_VER_RES
    #define MY_DISP_VER_RES    64
#endif

extern uint8_t ssd1306_buffer_dma[];

lv_display_t * disp_ssd1306 = NULL;

static void disp_init(void);
static void disp_flush(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map);

void lv_port_disp_init(void)
{
    LV_ATTRIBUTE_MEM_ALIGN static uint8_t buf_1[MY_DISP_HOR_RES * MY_DISP_VER_RES / 8 + 8];
    LV_ATTRIBUTE_MEM_ALIGN static uint8_t buf_2[MY_DISP_HOR_RES * MY_DISP_VER_RES / 8 + 8];
   
    /*-------------------------
     * Initialize your display
     * -----------------------*/
    disp_init();

    /*------------------------------------
     * Create a display and set a flush_cb
     * -----------------------------------*/
    lv_display_t * disp = lv_display_create(MY_DISP_HOR_RES, MY_DISP_VER_RES);
    disp_ssd1306 = disp;
    lv_display_set_flush_cb(disp, disp_flush);                                
    lv_display_set_buffers(disp, buf_1, buf_2, sizeof(buf_1), LV_DISPLAY_RENDER_MODE_FULL);
}

/*Initialize your display and the required peripherals.*/
static void disp_init(void)
{
    
}

static void disp_flush(lv_display_t * disp_drv, const lv_area_t * area, uint8_t * px_map)
{
    //uint16_t i,j = 0;

    px_map += 8;
    
    // 调试用，输出px_map的内容
    // printf("转换前的px_map\n");
    // for(i = 0; i < MY_DISP_HOR_RES * MY_DISP_VER_RES / 8; i++) {
    //     for(j = 0; j < 8; j++) {
    //         if(px_map[i] & (1 << (7-j))) {
    //             printf("1");
    //         } else {
    //            printf("0");
    //        }
    //    }
    // }
    // printf("\n");

    // 自定义转换代码，将LVGL的px_map转换为SSD1306能识别的格式
    // for(i = 0; i <  MY_DISP_HOR_RES * MY_DISP_VER_RES / 8; i++) {
    //     for(j = 0; j < 8; j++) {
    //         if(px_map[i] & (1 << (7-j))) {
    //             ssd1306_draw_pixel((i * 8 + j) % MY_DISP_HOR_RES, (i * 8 + j) / MY_DISP_HOR_RES, true);
    //         } else {
    //             ssd1306_draw_pixel((i * 8 + j) % MY_DISP_HOR_RES, (i * 8 + j) / MY_DISP_HOR_RES, false);
    //         }
    //     }
    // }        
    // ssd1306_flush(); //页寻址模式时可用
    // lv_display_flush_ready(disp_drv); 
    
    lv_draw_sw_i1_convert_to_vtiled(px_map, MY_DISP_HOR_RES * MY_DISP_VER_RES / 8, 
                                    MY_DISP_HOR_RES, MY_DISP_VER_RES, 
                                    ssd1306_buffer_dma, sizeof(ssd1306_buffer_dma),
                                    true);
    ssd1306_flush_dma();
}

#else /*Enable this file at the top*/

/*This dummy typedef exists purely to silence -Wpedantic.*/
typedef int keep_pedantic_happy;

#endif
