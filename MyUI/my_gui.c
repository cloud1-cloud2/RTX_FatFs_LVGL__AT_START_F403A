#include "my_gui.h"
#include "fan_ss6285l.h"
#include "sensor_aht20.h"
#include "lvgl.h"
#include "ff.h"
#include "calendar_rtc.h"
#include "usbdevice.h"
#include "voltage_adc.h"
#include "stdbool.h"
#include "stdio.h"
#include "string.h"

extern lv_indev_t * indev_encoder;

extern void fan_ui(void);
extern void temp_ui(void);
extern void humidity_ui(void);
extern void home_ui(void);
extern void file_ui(void);
extern void txt_popup_ui(void);
extern void time_ui(void);
extern void gamepad_ui(void);
extern void voltage_ui(void);

lv_theme_t * theme;
lv_timer_t * timer;
lv_group_t * group_home;
lv_group_t * group_fan;
lv_group_t * group_temp;
lv_group_t * group_humidity;
lv_group_t * group_file;
lv_group_t * group_txt_popup;
lv_group_t * group_time;
lv_group_t * group_gamepad;
lv_group_t * voltage_group;
lv_obj_t * scr_home;
lv_obj_t * scr_fan;
lv_obj_t * scr_temp;
lv_obj_t * scr_humidity;
lv_obj_t * scr_file;
lv_obj_t * scr_time;
lv_obj_t * label_temp;
lv_obj_t * label_humidity;
lv_obj_t * label_fan_speed_value;
lv_obj_t * label_current_date;
lv_obj_t * label_current_time;
lv_obj_t * ta_txt_popup;
lv_obj_t * file_explorer;
lv_obj_t * btn_temp_back;
lv_obj_t * btn_humidity_back;
lv_obj_t * btn_fan_back;
lv_obj_t * btn_file_back;
lv_obj_t * btn_time_back;
lv_obj_t * voltage_scr;
lv_obj_t * voltage_btn_back;
lv_obj_t * voltage_label_voltage;
lv_obj_t * gamepad_scr;
lv_obj_t * gamepad_btn_back;
lv_obj_t * gamepad_btn_up;
lv_obj_t * gamepad_btn_down;
lv_obj_t * gamepad_btn_left;
lv_obj_t * gamepad_btn_right;

int16_t fan_speed;
float temperature;
float humidity;
float voltage;

/*====================== ui entry ======================*/
static void timer_cb(lv_timer_t * timer)
{
    temperature = aht20_get_temperature(); //aht20_get_xxx()的实现没有延时80ms等待测量完成，而是直接读取上次测量结果
    humidity = aht20_get_humidity();
    voltage = voltage_adc_get_voltage_value();
    rtc_time_get(); //更新值到全局变量calendar
	
    if(lv_obj_is_valid(label_temp)) {
        lv_obj_send_event(label_temp, LV_EVENT_REFRESH, NULL);
    }

    if(lv_obj_is_valid(label_humidity)) {
        lv_obj_send_event(label_humidity, LV_EVENT_REFRESH, NULL);
    }

    if(lv_obj_is_valid(label_current_date)) {
        //lv_label_set_text_fmt(label_current_date, "%04d-%02d-%02d", calendar.year, calendar.month, calendar.date);
        lv_obj_send_event(label_current_date, LV_EVENT_REFRESH, NULL);
    }

    if(lv_obj_is_valid(label_current_time)) {
        //lv_label_set_text_fmt(label_current_time, "%02d:%02d:%02d", calendar.hour, calendar.min, calendar.sec);
        lv_obj_send_event(label_current_time, LV_EVENT_REFRESH, NULL);
    }

    if(lv_obj_is_valid(voltage_label_voltage)) {
        //lv_label_set_text_fmt(voltage_label_voltage, "V: %.2fV", voltage);
        lv_obj_send_event(voltage_label_voltage, LV_EVENT_REFRESH, NULL);
    }

    return;
}

void lvgl_ui(void)
{                 
    home_ui();
    timer = lv_timer_create(timer_cb, 1000,  NULL);
}

/*====================== home ui ======================*/
static void home_btn_temp_event_cb(lv_event_t * e)
{
    temp_ui();
}

static void home_btn_humi_event_cb(lv_event_t * e)
{
    humidity_ui();
}

static void home_btn_fan_event_cb(lv_event_t * e)
{
    fan_ui();
}

static void home_btn_file_event_cb(lv_event_t * e)
{
    file_ui();
}

static void home_btn_time_event_cb(lv_event_t * e)
{
    time_ui();
}

static void home_btn_gamepad_event_cb(lv_event_t * e)
{
    gamepad_ui();
}

static void home_btn_voltage_event_cb(lv_event_t * e)
{
    voltage_ui();
}

void home_ui()
{
    /* 设置单色主题 */
    theme = lv_theme_mono_init(lv_display_get_default(), true, &lv_font_montserrat_20);
    lv_display_set_theme(lv_display_get_default(), theme);

    /* 创建页面 */
    scr_home = lv_obj_create(NULL);

    lv_obj_t * scr_home_cont = lv_obj_create(scr_home);
    lv_obj_set_size(scr_home_cont, LV_PCT(100), LV_PCT(100));
    lv_obj_set_flex_flow(scr_home_cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_border_width(scr_home_cont, 0, 0);
    lv_obj_set_style_pad_top(scr_home_cont, 2, 0);
    lv_obj_set_style_pad_bottom(scr_home_cont, 8, 0);
    lv_obj_set_style_pad_left(scr_home_cont, 2, 0);

    lv_obj_t * btn_temp = lv_button_create(scr_home_cont);
    lv_obj_set_style_border_width(btn_temp, 0, 0);
    lv_obj_set_style_pad_hor(btn_temp, 0, 0);
    lv_obj_set_style_pad_ver(btn_temp, 2, 0);
    lv_obj_add_event_cb(btn_temp, home_btn_temp_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t * label1 = lv_label_create(btn_temp);
    lv_label_set_text(label1, "temp.");
    lv_obj_center(label1);

    lv_obj_t * btn_humidity = lv_button_create(scr_home_cont);
    lv_obj_set_style_border_width(btn_humidity, 0, 0);
    lv_obj_set_style_pad_hor(btn_humidity, 0, 0);
    lv_obj_set_style_pad_ver(btn_humidity, 2, 0);
    lv_obj_add_event_cb(btn_humidity, home_btn_humi_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t * label_humidity = lv_label_create(btn_humidity);
    lv_label_set_text(label_humidity, "humidity");
    lv_obj_center(label_humidity);

    lv_obj_t * btn_fan = lv_button_create(scr_home_cont);
    lv_obj_set_style_border_width(btn_fan, 0, 0);
    lv_obj_set_style_pad_hor(btn_fan, 0, 0);
    lv_obj_set_style_pad_ver(btn_fan, 2, 0);
    lv_obj_add_event_cb(btn_fan, home_btn_fan_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t * label_fan = lv_label_create(btn_fan);
    lv_label_set_text(label_fan, "fan");
    lv_obj_center(label_fan);
    
    lv_obj_t * btn_file = lv_button_create(scr_home_cont);
    lv_obj_set_style_border_width(btn_file, 0, 0);
    lv_obj_set_style_pad_hor(btn_file, 0, 0);
    lv_obj_set_style_pad_ver(btn_file, 2, 0);
    lv_obj_add_event_cb(btn_file, home_btn_file_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t * label_file = lv_label_create(btn_file);
    lv_label_set_text(label_file, "file");
    lv_obj_center(label_file);

    lv_obj_t * btn_time = lv_button_create(scr_home_cont);
    lv_obj_set_style_border_width(btn_time, 0, 0);
    lv_obj_set_style_pad_hor(btn_time, 0, 0);
    lv_obj_set_style_pad_ver(btn_time, 2, 0);
    lv_obj_add_event_cb(btn_time, home_btn_time_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t * label_time = lv_label_create(btn_time);
    lv_label_set_text(label_time, "time");
    lv_obj_center(label_time);

    lv_obj_t * btn_gamepad = lv_button_create(scr_home_cont);
    lv_obj_set_style_border_width(btn_gamepad, 0, 0);
    lv_obj_set_style_pad_hor(btn_gamepad, 0, 0);
    lv_obj_set_style_pad_ver(btn_gamepad, 2, 0);
    lv_obj_add_event_cb(btn_gamepad, home_btn_gamepad_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t * label_gamepad = lv_label_create(btn_gamepad);
    lv_label_set_text(label_gamepad, "gamepad");
    lv_obj_center(label_gamepad);

    lv_obj_t * btn_voltage = lv_button_create(scr_home_cont);
    lv_obj_set_style_border_width(btn_voltage, 0, 0);
    lv_obj_set_style_pad_hor(btn_voltage, 0, 0);
    lv_obj_set_style_pad_ver(btn_voltage, 2, 0);
    lv_obj_add_event_cb(btn_voltage, home_btn_voltage_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t * label_voltage = lv_label_create(btn_voltage);
    lv_label_set_text(label_voltage, "voltage");
    lv_obj_center(label_voltage);
    
    /* 加载页面 */
    lv_screen_load(scr_home);
    
    /* 设置group */
    group_home = lv_group_create();
    lv_group_add_obj(group_home, btn_temp);
    lv_group_add_obj(group_home, btn_humidity);
    lv_group_add_obj(group_home, btn_fan);
    lv_group_add_obj(group_home, btn_file);
    lv_group_add_obj(group_home, btn_time);
    lv_group_add_obj(group_home, btn_gamepad);    
    lv_group_add_obj(group_home, btn_voltage);
    lv_indev_set_group(indev_encoder, group_home); 
    lv_group_set_wrap(group_home, false);
    lv_group_focus_obj(btn_temp);
}


/*====================== temperature ui ======================*/
static void temp_label_event_cb(lv_event_t * e)
{
    lv_obj_t * label = lv_event_get_target_obj(e);
    lv_label_set_text_fmt(label, "T: %.1f°C", temperature); 
}

static void temp_btn_back_event_cb(lv_event_t * e)
{
    /* 切换到scr_home页面 */
    lv_screen_load(scr_home);
    lv_indev_set_group(indev_encoder, group_home);
    
    /* 删除temp_ui页面和group_temp组 */
    lv_obj_del(scr_temp);
    lv_group_del(group_temp);

    /* 切换到group_home组 */
    lv_indev_set_group(indev_encoder, group_home);
}

void temp_ui(void)
{
    /* 创建页面 */
    scr_temp = lv_obj_create(NULL);

    label_temp = lv_label_create(scr_temp);
    lv_obj_center(label_temp);
    lv_label_set_text(label_temp, "T: --.-°C");
    lv_obj_add_event_cb(label_temp, temp_label_event_cb, LV_EVENT_REFRESH, NULL);

    btn_temp_back = lv_button_create(scr_temp);
    lv_obj_add_event_cb(btn_temp_back, temp_btn_back_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_align(btn_temp_back, LV_ALIGN_BOTTOM_LEFT, 3, -3);
    lv_obj_set_size(btn_temp_back, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_style_border_width(btn_temp_back, 0, 0);
    lv_obj_set_style_pad_all(btn_temp_back, 0, 0);
    lv_obj_t * label_back = lv_label_create(btn_temp_back);
    lv_label_set_text(label_back, LV_SYMBOL_LEFT);

    /* 加载页面 */
    lv_screen_load(scr_temp);

    /* 设置group */
    group_temp = lv_group_create();
    lv_group_add_obj(group_temp, btn_temp_back);
    lv_group_set_wrap(group_temp, false);
    lv_indev_set_group(indev_encoder, group_temp);
    lv_group_focus_obj(btn_temp_back);
}


/*====================== humidity ui ======================*/
static void humidity_label_event_cb(lv_event_t * e)
{
    lv_obj_t * label = lv_event_get_target_obj(e);
    lv_label_set_text_fmt(label, "H: %.1f%%", humidity * 100); 
}

static void humidity_btn_back_event_cb(lv_event_t * e)
{
    /* 切换到scr_home页面 */
    lv_screen_load(scr_home);
    lv_indev_set_group(indev_encoder, group_home);

    /* 删除humidity_ui页面和group_humidity组 */
    lv_group_del(group_humidity);
    lv_obj_del(scr_humidity);

    /* 切换到group_home组 */
    lv_indev_set_group(indev_encoder, group_home);
}

void humidity_ui(void)
{
    /* 创建页面 */
    scr_humidity = lv_obj_create(NULL);

    label_humidity = lv_label_create(scr_humidity);
    lv_obj_center(label_humidity);
    lv_label_set_text(label_humidity, "H: --.-%");
    lv_obj_add_event_cb(label_humidity, humidity_label_event_cb, LV_EVENT_REFRESH, NULL);

    btn_humidity_back = lv_button_create(scr_humidity);
    lv_obj_add_event_cb(btn_humidity_back, humidity_btn_back_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_align(btn_humidity_back, LV_ALIGN_BOTTOM_LEFT, 3, -3);
    lv_obj_set_size(btn_humidity_back, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_style_border_width(btn_humidity_back, 0, 0);
    lv_obj_set_style_pad_all(btn_humidity_back, 0, 0);
    lv_obj_t * label_back = lv_label_create(btn_humidity_back);
    lv_label_set_text(label_back, LV_SYMBOL_LEFT);

    /* 加载页面 */
    lv_screen_load(scr_humidity);

    /* 设置group */
    group_humidity = lv_group_create();
    lv_group_add_obj(group_humidity, btn_humidity_back);
    lv_group_set_wrap(group_humidity, false);
    lv_indev_set_group(indev_encoder, group_humidity);
    lv_group_focus_obj(btn_humidity_back);
}


/*====================== fan ui ======================*/
static void fan_slider_event_cb(lv_event_t * e)
{
    fan_speed = lv_slider_get_value(lv_event_get_target_obj(e));
    fan_set_speed(fan_speed);
    lv_label_set_text_fmt(label_fan_speed_value, "%d", fan_speed);
}

static void fan_btn_back_event_cb(lv_event_t * e)
{
    /* 切换到scr_home页面 */
    lv_screen_load(scr_home);
    lv_indev_set_group(indev_encoder, group_home);

    /* 删除fan_ui页面和group_fan组 */
    lv_group_del(group_fan);
    lv_obj_del(scr_fan);

    /* 切换到group_home组 */
    lv_indev_set_group(indev_encoder, group_home);
}

void fan_ui(void) 
{
    /* 创建页面 */
    scr_fan = lv_obj_create(NULL);

    lv_obj_t * slider_fan_speed = lv_slider_create(scr_fan);
    lv_obj_set_size(slider_fan_speed, LV_PCT(60), LV_PCT(30));
    lv_obj_align(slider_fan_speed, LV_ALIGN_BOTTOM_RIGHT, -13, -5);
    lv_obj_set_style_radius(slider_fan_speed, 3, LV_PART_INDICATOR);
    lv_obj_set_style_radius(slider_fan_speed, 3, LV_PART_KNOB);
    lv_obj_set_style_bg_color(slider_fan_speed, lv_color_hex(0xFFFFFF), LV_PART_KNOB);
    lv_obj_set_style_bg_opa(slider_fan_speed, LV_OPA_COVER, LV_PART_KNOB);
    lv_slider_set_range(slider_fan_speed, -5, 5);
    lv_slider_set_value(slider_fan_speed, fan_speed, LV_ANIM_OFF);
    lv_obj_add_event_cb(slider_fan_speed, fan_slider_event_cb, LV_EVENT_VALUE_CHANGED, NULL);

    lv_obj_t * label_speed = lv_label_create(scr_fan);
    lv_label_set_text(label_speed, "speed");
    lv_obj_align_to(label_speed, slider_fan_speed, LV_ALIGN_OUT_TOP_MID, 0, -5);

    label_fan_speed_value = lv_label_create(scr_fan);
    lv_label_set_text_fmt(label_fan_speed_value, "%d", fan_speed);
    lv_obj_align(label_fan_speed_value, LV_ALIGN_TOP_LEFT, 3, 3);

    btn_fan_back = lv_button_create(scr_fan);
    lv_obj_add_event_cb(btn_fan_back, fan_btn_back_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_align(btn_fan_back, LV_ALIGN_BOTTOM_LEFT, 3, -3);
    lv_obj_set_size(btn_fan_back, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_style_border_width(btn_fan_back, 0, 0);
    lv_obj_set_style_pad_all(btn_fan_back, 0, 0);
    lv_obj_t * label_back = lv_label_create(btn_fan_back);
    lv_label_set_text(label_back, LV_SYMBOL_LEFT);

    /* 加载页面 */
    lv_screen_load(scr_fan);

    /* 设置group */
    group_fan = lv_group_create();
    lv_group_add_obj(group_fan, btn_fan_back);
    lv_group_add_obj(group_fan, slider_fan_speed);
    lv_group_set_wrap(group_fan, false);
    lv_indev_set_group(indev_encoder, group_fan);
    lv_group_focus_obj(btn_fan_back);
}


/*====================== file system ui ======================*/
static void file_btn_back_event_cb(lv_event_t * e)
{
    /* 切换到scr_home页面 */
    lv_screen_load(scr_home);
    lv_indev_set_group(indev_encoder, group_home);

    /* 删除file_ui页面和group_file组 */
    lv_group_del(group_file);
    lv_obj_del(scr_file);

    /* 切换到group_home组 */
    lv_indev_set_group(indev_encoder, group_home);
}

static void file_file_explorer_event_cb(lv_event_t * e)
{
    lv_fs_file_t f;
    lv_fs_res_t res;
    uint32_t read_num;
    uint8_t buf[128];
    const char * cur_path;
    const char * sel_fn;
    char full_path[LV_FILE_EXPLORER_PATH_MAX_LEN];

    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);

    if(code == LV_EVENT_VALUE_CHANGED) {
        cur_path =  lv_file_explorer_get_current_path(file_explorer);
        sel_fn = lv_file_explorer_get_selected_file_name(file_explorer);
        snprintf(full_path, sizeof(full_path), "%s%s", cur_path, sel_fn);

        if(strstr(sel_fn, ".txt") != NULL) { //如果选中的是以.TXT结尾的文件
            txt_popup_ui(); //打开txt文件弹窗
           
            //读取文件内容并更新到文本区域
            res = lv_fs_open(&f, full_path, LV_FS_MODE_RD);
            if(res != LV_FS_RES_OK) printf("Failed to open file. res=%d\n", res);

            res = lv_fs_read(&f, buf, sizeof(buf) - 1, &read_num);
            if(res != LV_FS_RES_OK) printf("Failed to read file. res=%d\n", res);

            buf[read_num] = '\0'; //确保字符串以null结尾
            lv_textarea_set_text(ta_txt_popup, (char*)buf);
            
            lv_fs_close(&f);
        }
    }
}

void file_ui(void)
{
    /* 创建页面 */
    scr_file = lv_obj_create(NULL);

    lv_obj_t * scr_file_cont = lv_obj_create(scr_file);
    lv_obj_set_size(scr_file_cont, LV_PCT(100), LV_PCT(100));
    lv_obj_set_pos(scr_file_cont, 0, 0);
    lv_obj_set_style_border_width(scr_file_cont, 0, 0);
    lv_obj_set_flex_flow(scr_file_cont, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(scr_file_cont, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_END, LV_FLEX_ALIGN_END);

    btn_file_back = lv_button_create(scr_file_cont);
    lv_obj_add_event_cb(btn_file_back, file_btn_back_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_set_size(btn_file_back, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_style_border_width(btn_file_back, 0, 0);
    lv_obj_set_style_pad_all(btn_file_back, 0, 0);
    lv_obj_t * label_back = lv_label_create(btn_file_back);
    lv_label_set_text(label_back, LV_SYMBOL_LEFT);

    file_explorer = lv_file_explorer_create(scr_file_cont);
    lv_obj_set_size(file_explorer, LV_PCT(80), LV_PCT(100));
    lv_file_explorer_set_sort(file_explorer, LV_EXPLORER_SORT_KIND);
    lv_obj_add_event_cb(file_explorer, file_file_explorer_event_cb, LV_EVENT_ALL, NULL);
    lv_file_explorer_open_dir(file_explorer, "F:0:/");

    //获取头部
    lv_obj_t * header = lv_file_explorer_get_header(file_explorer);
    lv_obj_set_size(header, LV_PCT(100), LV_PCT(30));
    lv_obj_set_style_text_font(header, &lv_font_montserrat_16, 0);

    //获取文件表
    lv_obj_t * file_table   = lv_file_explorer_get_file_table(file_explorer);
    lv_obj_set_size(file_table, LV_PCT(100), LV_PCT(70));
    lv_obj_set_style_text_font(file_table, &lv_font_montserrat_16, 0);

    /* 加载页面 */
    lv_screen_load(scr_file);

    /* 设置group */
    group_file = lv_group_create();
    lv_group_add_obj(group_file, btn_file_back);
    lv_group_add_obj(group_file, file_table);
    lv_group_set_wrap(group_file, false);
    lv_indev_set_group(indev_encoder, group_file); 
    lv_group_focus_obj(btn_file_back);
}


/*====================== txt file open popup ui ======================*/
static void file_file_explorer_txt_file_open_pupup_btn_back_event_cb(lv_event_t *e)
{
    /* 删除页面 */
    lv_obj_t *container = lv_obj_get_parent(lv_event_get_target(e));
    lv_obj_del(container);

    /* 切换到group_file组 */
    lv_group_del(group_txt_popup);
    lv_indev_set_group(indev_encoder, group_file);
}

void txt_popup_ui(void)
{
    /* 创建页面 */
    lv_obj_t *container = lv_obj_create(lv_scr_act());
    lv_obj_set_size(container, LV_PCT(85), LV_PCT(85));
    lv_obj_align(container, LV_ALIGN_CENTER, 0, 0);

    ta_txt_popup = lv_textarea_create(container);
    lv_obj_set_size(ta_txt_popup, LV_PCT(70), LV_PCT(100));
    lv_obj_align(ta_txt_popup, LV_ALIGN_RIGHT_MID, 0, 0);
    lv_obj_set_style_border_width(ta_txt_popup, 0, 0);
    lv_textarea_set_placeholder_text(ta_txt_popup, "");     // 清空提示文本

    lv_obj_t *btn_back = lv_button_create(container);
    lv_obj_set_size(btn_back, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_style_border_width(btn_back, 0, 0);
    lv_obj_set_style_pad_all(btn_back, 0, 0);
    lv_obj_align(btn_back, LV_ALIGN_BOTTOM_LEFT, 0, 0);
    lv_obj_t * label_back = lv_label_create(btn_back);
    lv_label_set_text(label_back, LV_SYMBOL_LEFT);
    lv_obj_add_event_cb(btn_back, file_file_explorer_txt_file_open_pupup_btn_back_event_cb, LV_EVENT_CLICKED, NULL);

    /* 设置group */
    group_txt_popup = lv_group_create();
    lv_group_add_obj(group_txt_popup, btn_back);
    lv_group_add_obj(group_txt_popup, ta_txt_popup);
    lv_group_set_wrap(group_txt_popup, false);
    lv_indev_set_group(indev_encoder, group_txt_popup);
    lv_group_focus_obj(btn_back);
}

/*====================== time ui ======================*/
static void time_label_current_date_event_cb(lv_event_t * e)
{
    lv_obj_t * label = lv_event_get_target_obj(e);
    lv_label_set_text_fmt(label, "%04d-%02d-%02d", calendar.year, calendar.month, calendar.date);
}

static void time_label_current_time_event_cb(lv_event_t * e)
{
    lv_obj_t * label = lv_event_get_target_obj(e);
    lv_label_set_text_fmt(label, "%02d:%02d:%02d", calendar.hour, calendar.min, calendar.sec);
}

static void time_btn_back_event_cb(lv_event_t * e)
{
    /* 切换到scr_home页面 */
    lv_screen_load(scr_home);
    lv_indev_set_group(indev_encoder, group_home);

    /* 删除time_ui页面和group_time组 */
    lv_group_del(group_time);
    lv_obj_del(scr_time);

    /* 切换到group_home组 */
    lv_indev_set_group(indev_encoder, group_home);
}

void time_ui(void)
{
    /* 创建页面 */
    scr_time = lv_obj_create(NULL);

    label_current_date = lv_label_create(scr_time);
    lv_obj_align(label_current_date, LV_ALIGN_CENTER, 0, -10);
    lv_label_set_text(label_current_date, "0000-00-00");
    lv_obj_add_event_cb(label_current_date, time_label_current_date_event_cb, LV_EVENT_REFRESH, NULL);

    label_current_time = lv_label_create(scr_time);
    lv_obj_align(label_current_time, LV_ALIGN_CENTER, 0, 10);
    lv_label_set_text(label_current_time, "00:00:00");
    lv_obj_add_event_cb(label_current_time, time_label_current_time_event_cb, LV_EVENT_REFRESH, NULL);

    btn_time_back = lv_button_create(scr_time);
    lv_obj_add_event_cb(btn_time_back, time_btn_back_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_align(btn_time_back, LV_ALIGN_BOTTOM_LEFT, 3, -3);
    lv_obj_set_size(btn_time_back, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_style_border_width(btn_time_back, 0, 0);
    lv_obj_set_style_pad_all(btn_time_back, 0, 0);
    lv_obj_t * label_back = lv_label_create(btn_time_back);
    lv_label_set_text(label_back, LV_SYMBOL_LEFT);

    /* 加载页面 */
    lv_screen_load(scr_time);

    /* 设置group */
    group_time = lv_group_create();
    lv_group_add_obj(group_time, btn_time_back);
    lv_group_set_wrap(group_time, false);
    lv_indev_set_group(indev_encoder, group_time);
    lv_group_focus_obj(btn_time_back);
}

/*====================== gamepad ui ======================*/
static void gamepad_btn_back_event_cb(lv_event_t * e)
{
    /* 切换到scr_home页面 */
    lv_screen_load(scr_home);
    lv_indev_set_group(indev_encoder, group_home);

    /* 删除gamepad_ui页面和group_gamepad组 */
    lv_group_del(group_gamepad);
    lv_obj_del(gamepad_scr);

    /* 切换到group_home组 */
    lv_indev_set_group(indev_encoder, group_home);
}

static void gamepad_btn_up_pressed_event_cb(lv_event_t * e)
{
    report1.dpad = 1; //上
    custom_hid_class_send_report(&usb_core_dev, (uint8_t*)&report1, sizeof(report1));
}

static void gamepad_btn_up_released_event_cb(lv_event_t * e)
{
    report1.dpad = 0x00; //中立
    custom_hid_class_send_report(&usb_core_dev, (uint8_t*)&report1, sizeof(report1));
}

static void gamepad_btn_down_pressed_event_cb(lv_event_t * e)
{
    report1.dpad = 5; //下
    custom_hid_class_send_report(&usb_core_dev, (uint8_t*)&report1, sizeof(report1));
}

static void gamepad_btn_down_released_event_cb(lv_event_t * e)
{
    report1.dpad = 0; //中立
    custom_hid_class_send_report(&usb_core_dev, (uint8_t*)&report1, sizeof(report1));
}

static void gamepad_btn_left_pressed_event_cb(lv_event_t * e)
{
    report1.dpad = 7; //左
    custom_hid_class_send_report(&usb_core_dev, (uint8_t*)&report1, sizeof(report1));
}

static void gamepad_btn_left_released_event_cb(lv_event_t * e)
{
    report1.dpad = 0; //中立
    custom_hid_class_send_report(&usb_core_dev, (uint8_t*)&report1, sizeof(report1));
}

static void gamepad_btn_right_pressed_event_cb(lv_event_t * e)
{
    report1.dpad = 3; //右
    custom_hid_class_send_report(&usb_core_dev, (uint8_t*)&report1, sizeof(report1));
}

static void gamepad_btn_right_released_event_cb(lv_event_t * e)
{
    report1.dpad = 0; //中立
    custom_hid_class_send_report(&usb_core_dev, (uint8_t*)&report1, sizeof(report1));
}

void gamepad_ui(void)
{
    /* 创建页面 */
    gamepad_scr = lv_obj_create(NULL);

    gamepad_btn_back = lv_button_create(gamepad_scr);
    lv_obj_add_event_cb(gamepad_btn_back, gamepad_btn_back_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_align(gamepad_btn_back, LV_ALIGN_BOTTOM_LEFT, 3, -3);
    lv_obj_set_size(gamepad_btn_back, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_style_border_width(gamepad_btn_back, 0, 0);
    lv_obj_set_style_pad_all(gamepad_btn_back, 0, 0);
    lv_obj_t * label_back = lv_label_create(gamepad_btn_back);
    lv_label_set_text(label_back, LV_SYMBOL_LEFT);

    //上下左右按钮(按钮大小25x25)
    gamepad_btn_up = lv_button_create(gamepad_scr);
    lv_obj_set_size(gamepad_btn_up, 25, 25);
    lv_obj_align(gamepad_btn_up, LV_ALIGN_CENTER, 0, -15);
    lv_obj_add_event_cb(gamepad_btn_up, gamepad_btn_up_pressed_event_cb, LV_EVENT_PRESSED, NULL);
    lv_obj_add_event_cb(gamepad_btn_up, gamepad_btn_up_released_event_cb, LV_EVENT_RELEASED, NULL);
    lv_obj_t * label_up = lv_label_create(gamepad_btn_up);
    lv_label_set_text(label_up, "U");
    lv_obj_center(label_up);

    gamepad_btn_down = lv_button_create(gamepad_scr);
    lv_obj_set_size(gamepad_btn_down, 25, 25);
    lv_obj_align(gamepad_btn_down, LV_ALIGN_CENTER, 0, 15);
    lv_obj_add_event_cb(gamepad_btn_down, gamepad_btn_down_pressed_event_cb, LV_EVENT_PRESSED, NULL);
    lv_obj_add_event_cb(gamepad_btn_down, gamepad_btn_down_released_event_cb, LV_EVENT_RELEASED, NULL);
    lv_obj_t * label_down = lv_label_create(gamepad_btn_down);
    lv_label_set_text(label_down, "D");
    lv_obj_center(label_down);

    gamepad_btn_left = lv_button_create(gamepad_scr);
    lv_obj_set_size(gamepad_btn_left, 25, 25);
    lv_obj_align(gamepad_btn_left, LV_ALIGN_CENTER, -30, 0);
    lv_obj_add_event_cb(gamepad_btn_left, gamepad_btn_left_pressed_event_cb, LV_EVENT_PRESSED, NULL);
    lv_obj_add_event_cb(gamepad_btn_left, gamepad_btn_left_released_event_cb, LV_EVENT_RELEASED, NULL);
    lv_obj_t * label_left = lv_label_create(gamepad_btn_left);
    lv_label_set_text(label_left, "L");
    lv_obj_center(label_left);

    gamepad_btn_right = lv_button_create(gamepad_scr);
    lv_obj_set_size(gamepad_btn_right, 25, 25);
    lv_obj_align(gamepad_btn_right, LV_ALIGN_CENTER, 30, 0);
    lv_obj_add_event_cb(gamepad_btn_right, gamepad_btn_right_pressed_event_cb, LV_EVENT_PRESSED, NULL);
    lv_obj_add_event_cb(gamepad_btn_right, gamepad_btn_right_released_event_cb, LV_EVENT_RELEASED, NULL);
    lv_obj_t * label_right = lv_label_create(gamepad_btn_right);
    lv_label_set_text(label_right, "R");
    lv_obj_center(label_right);

    /* 加载页面 */
    lv_screen_load(gamepad_scr);

    /* 设置group */
    group_gamepad = lv_group_create();
    lv_group_add_obj(group_gamepad, gamepad_btn_back);
    lv_group_add_obj(group_gamepad, gamepad_btn_up);
    lv_group_add_obj(group_gamepad, gamepad_btn_right);
    lv_group_add_obj(group_gamepad, gamepad_btn_down);
    lv_group_add_obj(group_gamepad, gamepad_btn_left);
    lv_group_set_wrap(group_gamepad, false);
    lv_indev_set_group(indev_encoder, group_gamepad);
    lv_group_focus_obj(gamepad_btn_back);
}

/*====================== voltage ui ======================*/
static void voltage_label_voltage_event_cb(lv_event_t * e)
{
    lv_obj_t * label = lv_event_get_target_obj(e);
    lv_label_set_text_fmt(label, "V: %.2fV", voltage); 
}

static void voltage_btn_back_event_cb(lv_event_t * e)
{
    /* 切换到scr_home页面 */
    lv_screen_load(scr_home);
    lv_indev_set_group(indev_encoder, group_home);

    /* 删除voltage_ui页面和group_voltage组 */
    lv_group_del(voltage_group);
    lv_obj_del(voltage_scr);

    /* 切换到group_home组 */
    lv_indev_set_group(indev_encoder, group_home);
}

void voltage_ui(void)
{
    /* 创建页面 */
    voltage_scr = lv_obj_create(NULL);

    voltage_label_voltage = lv_label_create(voltage_scr);
    lv_obj_center(voltage_label_voltage);
    lv_label_set_text(voltage_label_voltage, "V: --.-V");
    lv_obj_add_event_cb(voltage_label_voltage, voltage_label_voltage_event_cb, LV_EVENT_REFRESH, NULL);

    voltage_btn_back = lv_button_create(voltage_scr);
    lv_obj_add_event_cb(voltage_btn_back, voltage_btn_back_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_align(voltage_btn_back, LV_ALIGN_BOTTOM_LEFT, 3, -3);
    lv_obj_set_size(voltage_btn_back, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_style_border_width(voltage_btn_back, 0, 0);
    lv_obj_set_style_pad_all(voltage_btn_back, 0, 0);
    lv_obj_t * label_back = lv_label_create(voltage_btn_back);
    lv_label_set_text(label_back, LV_SYMBOL_LEFT);

    /* 加载页面 */
    lv_screen_load(voltage_scr);

    /* 设置group */
    voltage_group = lv_group_create();
    lv_group_add_obj(voltage_group, voltage_btn_back);
    lv_group_set_wrap(voltage_group, false);
    lv_indev_set_group(indev_encoder, voltage_group);
    lv_group_focus_obj(voltage_btn_back);
}