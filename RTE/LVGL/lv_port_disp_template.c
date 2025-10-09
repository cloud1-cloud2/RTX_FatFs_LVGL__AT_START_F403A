/**
 * @file lv_port_disp_templ.c
 *
 */

/*Copy this file as "lv_port_disp.c" and set this value to "1" to enable content*/

#if 1

/*********************
 *      INCLUDES
 *********************/
#include "lv_port_disp_template.h"


/*********************
 *      DEFINES
 *********************/
#ifndef MY_DISP_HOR_RES
    #define MY_DISP_HOR_RES    128
#endif

#ifndef MY_DISP_VER_RES
    #define MY_DISP_VER_RES    64
#endif

/**********************
 *      TYPEDEFS 
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void disp_init(void);

static void disp_flush(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *  GLOBAL VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_port_disp_init(void)
{
    LV_ATTRIBUTE_MEM_ALIGN static uint8_t buf_1[MY_DISP_HOR_RES * MY_DISP_VER_RES / 8 + 8];
   
    /*-------------------------
     * Initialize your display
     * -----------------------*/
    disp_init();

    /*------------------------------------
     * Create a display and set a flush_cb
     * -----------------------------------*/
    lv_display_t * disp = lv_display_create(MY_DISP_HOR_RES, MY_DISP_VER_RES);
    lv_display_set_flush_cb(disp, disp_flush);                                
    lv_display_set_buffers(disp, buf_1, NULL, sizeof(buf_1), LV_DISPLAY_RENDER_MODE_FULL);
    
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/*Initialize your display and the required peripherals.*/
static void disp_init(void)
{
    
}

static void disp_flush(lv_display_t * disp_drv, const lv_area_t * area, uint8_t * px_map)
{
    uint16_t i,j = 0;

    px_map += 8;
    
//     调试用，输出px_map的内容
//     printf("转换前的px_map\n");
//     for(i = 0; i < MY_DISP_HOR_RES * MY_DISP_VER_RES / 8; i++) {
//         for(j = 0; j < 8; j++) {
//             if(px_map[i] & (1 << (7-j))) {
//                 printf("1");
//             } else {
//                printf("0");
//            }
//        }
//    }
//    printf("\n");

    for(i = 0; i <  MY_DISP_HOR_RES * MY_DISP_VER_RES / 8; i++) {
        for(j = 0; j < 8; j++) {
            if(px_map[i] & (1 << (7-j))) {
                ssd1306_draw_pixel((i * 8 + j) % MY_DISP_HOR_RES, (i * 8 + j) / MY_DISP_HOR_RES, true);
            } else {
                ssd1306_draw_pixel((i * 8 + j) % MY_DISP_HOR_RES, (i * 8 + j) / MY_DISP_HOR_RES, false);
            }
        }
        
    }
        
    ssd1306_flush();
    
    lv_display_flush_ready(disp_drv);   
}

#else /*Enable this file at the top*/

/*This dummy typedef exists purely to silence -Wpedantic.*/
typedef int keep_pedantic_happy;
#endif
