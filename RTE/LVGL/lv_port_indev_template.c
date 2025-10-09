/**
 * @file lv_port_indev_templ.c
 *
 */

/*Copy this file as "lv_port_indev.c" and set this value to "1" to enable content*/

#if 1

/*********************
 *      INCLUDES
 *********************/
#include "lv_port_indev_template.h"

/*********************
 *      DEFINES
 *********************/
#define ENCODER_USED_TMR TMR4
#define ENCODER_KEY_PORT GPIOB
#define ENCODER_KEY_PIN GPIO_PINS_5

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void encoder_init(void);
static void encoder_read(lv_indev_t * indev, lv_indev_data_t * data);

/**********************
 *  STATIC VARIABLES
 **********************/
lv_indev_t * indev_encoder;

static int32_t encoder_diff = 0;
static lv_indev_state_t encoder_state = LV_INDEV_STATE_RELEASED;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_port_indev_init(void)
{
    /**
     * Here you will find example implementation of input devices supported by LittelvGL:
     *  - Touchpad
     *  - Mouse (with cursor support)
     *  - Keypad (supports GUI usage only with key)
     *  - Encoder (supports GUI usage only with: left, right, push)
     *  - Button (external buttons to press points on the screen)
     *
     *  The `..._read()` function are only examples.
     *  You should shape them according to your hardware
     */

    /*------------------
     * Encoder
     * -----------------*/

    /*Initialize your encoder if you have*/
    encoder_init();

    /*Register a encoder input device*/
    indev_encoder = lv_indev_create();
    
    lv_indev_set_type(indev_encoder, LV_INDEV_TYPE_ENCODER);
    lv_indev_set_read_cb(indev_encoder, encoder_read);

    /*Later you should create group(s) with `lv_group_t * group = lv_group_create()`,
     *add objects to the group with `lv_group_add_obj(group, obj)`
     *and assign this input device to group to navigate in it:
     *`lv_indev_set_group(indev_encoder, group);`*/

}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/*------------------
 * Encoder
 * -----------------*/

/*Initialize your encoder*/
static void encoder_init(void)
{
    
}

/*Will be called by the library to read the encoder*/
static void encoder_read(lv_indev_t * indev_drv, lv_indev_data_t * data)
{
    //计算encoder_diff
    static int16_t encoder_value_last = 0;
    int16_t encoder_value_current;
    
    encoder_value_current = (int16_t)tmr_counter_value_get(ENCODER_USED_TMR);
    encoder_diff = encoder_value_current - encoder_value_last;


    // 边界修正
    if(encoder_diff > 32767) {
        encoder_diff -= 65536;
    } else if(encoder_diff < -32768) {
        encoder_diff += 65536;
    }
    
    //计算encoder_state
    if(gpio_input_data_bit_read(ENCODER_KEY_PORT, ENCODER_KEY_PIN) == RESET ){
        encoder_state = LV_INDEV_STATE_PRESSED;
    }else{
        encoder_state = LV_INDEV_STATE_RELEASED;
    }
//    //调试用
//    if(encoder_diff!=0)
//    {
//        printf("encoder_value_last = %d\n", encoder_value_last);
//        printf("encoder_value_current = %d\n", encoder_value_current);
//        printf("encoder_diff = %d\n", encoder_diff);
//        
//        if(encoder_state == LV_INDEV_STATE_PRESSED)
//        {
//            printf("encoder_state = LV_INDEV_STATE_PRESSED\n");
//        }else{
//            printf("encoder_state = LV_INDEV_STATE_RELEASED\n");
//        }
//        
//    }
    
    encoder_value_last = encoder_value_current;
    
    data->enc_diff = encoder_diff;
    data->state = encoder_state;
}

#else /*Enable this file at the top*/

/*This dummy typedef exists purely to silence -Wpedantic.*/
typedef int keep_pedantic_happy;
#endif
