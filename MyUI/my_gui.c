#include "my_gui.h"
#include "fan_ss6285l.h"
#include "sensor_aht20.h"
#include "lvgl.h"
#include "lv_port_indev.h" // extern indev_encoder
#include "ff.h"
#include "calendar_rtc.h" // extern calendar_type calendar;
#include "usbdevice.h"
#include "voltage_adc.h"
#include "stdbool.h"
#include "stdio.h"
#include "string.h"

lv_theme_t * theme;
lv_timer_t * timer;

// home UI member
lv_obj_t * home_scr;
lv_group_t * home_group;
void home_ui(void);

// temperature UI member
lv_obj_t * temp_scr;
lv_obj_t * temp_btn_back;
lv_obj_t * temp_label;
float temperature;
lv_group_t * temp_group;
void temp_ui(void);

// humidity UI member
lv_obj_t * humidity_scr;
lv_obj_t * humidity_btn_back;
lv_obj_t * humidity_label;
float humidity;
lv_group_t * humidity_group;
void humidity_ui(void);

// Fan UI member
lv_obj_t * fan_scr;
lv_obj_t * fan_btn_back;
lv_obj_t * fan_label_speed_value;
int16_t fan_speed;
lv_group_t * fan_group;
void fan_ui(void);

// File Explorer UI member
lv_obj_t * file_scr;
lv_obj_t * file_explorer;
lv_group_t * file_group;
char file_file_table_previous_selected_cell_value[128]; // 存储file_table当前被选中单元格的值
lv_file_explorer_file_table_entry_data_t file_file_table_previous_entry_user_data; // 存储file_table当前被选中单元格的用户数据
char file_previous_path[128]; // 存储当前路径
void file_ui(void);

// Txt file popup UI member
lv_obj_t * txt_popup_ta; 
lv_group_t * txt_popup_group;
void txt_popup_ui(void);

// Bin file popup UI member
lv_obj_t * bin_popup_img_placeholder;
lv_group_t * bin_popup_group;
void bin_popup_ui(void);

// Time UI member
lv_obj_t * time_scr;
lv_obj_t * time_btn_back;
lv_obj_t * time_label_current_date;
lv_obj_t * time_label_current_time;
lv_group_t * time_group;
void time_ui(void);

// Voltage UI member
lv_obj_t * voltage_scr;
lv_obj_t * voltage_btn_back;
lv_obj_t * voltage_label_voltage;
float voltage;
lv_group_t * voltage_group;
void voltage_ui(void);

// Gamepad UI member
lv_obj_t * gamepad_scr;
lv_obj_t * gamepad_btn_back;
lv_obj_t * gamepad_btn_up;
lv_obj_t * gamepad_btn_down;
lv_obj_t * gamepad_btn_left;
lv_obj_t * gamepad_btn_right;
lv_group_t * gamepad_group;
void gamepad_ui(void);

/*====================== timer callback ======================*/

static void timer_cb(lv_timer_t * timer)
{
    temperature = aht20_get_temperature(); //aht20_get_xxx()的实现没有延时80ms等待测量完成，而是直接读取上次测量结果
    humidity = aht20_get_humidity();
    voltage = voltage_adc_get_voltage_value();
    rtc_time_get(); //更新值到全局变量calendar
	
    if(lv_obj_is_valid(temp_label)) {
        lv_obj_send_event(temp_label, LV_EVENT_REFRESH, NULL);
    }

    if(lv_obj_is_valid(humidity_label)) {
        lv_obj_send_event(humidity_label, LV_EVENT_REFRESH, NULL);
    }

    if(lv_obj_is_valid(time_label_current_date)) {
        //lv_label_set_text_fmt(time_label_current_date, "%04d-%02d-%02d", calendar.year, calendar.month, calendar.date);
        lv_obj_send_event(time_label_current_date, LV_EVENT_REFRESH, NULL);
    }

    if(lv_obj_is_valid(time_label_current_time)) {
        //lv_label_set_text_fmt(time_label_current_time, "%02d:%02d:%02d", calendar.hour, calendar.min, calendar.sec);
        lv_obj_send_event(time_label_current_time, LV_EVENT_REFRESH, NULL);
    }

    if(lv_obj_is_valid(voltage_label_voltage)) {
        //lv_label_set_text_fmt(voltage_label_voltage, "V: %.2fV", voltage);
        lv_obj_send_event(voltage_label_voltage, LV_EVENT_REFRESH, NULL);
    }

    return;
}

/*====================== ui entry ======================*/

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
    home_scr = lv_obj_create(NULL);

    lv_obj_t * home_scr_cont = lv_obj_create(home_scr);
    lv_obj_set_size(home_scr_cont, LV_PCT(100), LV_PCT(100));
    lv_obj_set_flex_flow(home_scr_cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_border_width(home_scr_cont, 0, 0);
    lv_obj_set_style_pad_top(home_scr_cont, 2, 0);
    lv_obj_set_style_pad_bottom(home_scr_cont, 8, 0);
    lv_obj_set_style_pad_left(home_scr_cont, 2, 0);

    lv_obj_t * btn_temp = lv_button_create(home_scr_cont);
    lv_obj_set_style_border_width(btn_temp, 0, 0);
    lv_obj_set_style_pad_hor(btn_temp, 0, 0);
    lv_obj_set_style_pad_ver(btn_temp, 2, 0);
    lv_obj_add_event_cb(btn_temp, home_btn_temp_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t * label1 = lv_label_create(btn_temp);
    lv_label_set_text(label1, "temp.");
    lv_obj_center(label1);

    lv_obj_t * btn_humidity = lv_button_create(home_scr_cont);
    lv_obj_set_style_border_width(btn_humidity, 0, 0);
    lv_obj_set_style_pad_hor(btn_humidity, 0, 0);
    lv_obj_set_style_pad_ver(btn_humidity, 2, 0);
    lv_obj_add_event_cb(btn_humidity, home_btn_humi_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t * humidity_label = lv_label_create(btn_humidity);
    lv_label_set_text(humidity_label, "humidity");
    lv_obj_center(humidity_label);

    lv_obj_t * btn_fan = lv_button_create(home_scr_cont);
    lv_obj_set_style_border_width(btn_fan, 0, 0);
    lv_obj_set_style_pad_hor(btn_fan, 0, 0);
    lv_obj_set_style_pad_ver(btn_fan, 2, 0);
    lv_obj_add_event_cb(btn_fan, home_btn_fan_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t * label_fan = lv_label_create(btn_fan);
    lv_label_set_text(label_fan, "fan");
    lv_obj_center(label_fan);
    
    lv_obj_t * btn_file = lv_button_create(home_scr_cont);
    lv_obj_set_style_border_width(btn_file, 0, 0);
    lv_obj_set_style_pad_hor(btn_file, 0, 0);
    lv_obj_set_style_pad_ver(btn_file, 2, 0);
    lv_obj_add_event_cb(btn_file, home_btn_file_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t * label_file = lv_label_create(btn_file);
    lv_label_set_text(label_file, "file");
    lv_obj_center(label_file);

    lv_obj_t * btn_time = lv_button_create(home_scr_cont);
    lv_obj_set_style_border_width(btn_time, 0, 0);
    lv_obj_set_style_pad_hor(btn_time, 0, 0);
    lv_obj_set_style_pad_ver(btn_time, 2, 0);
    lv_obj_add_event_cb(btn_time, home_btn_time_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t * label_time = lv_label_create(btn_time);
    lv_label_set_text(label_time, "time");
    lv_obj_center(label_time);

    lv_obj_t * btn_gamepad = lv_button_create(home_scr_cont);
    lv_obj_set_style_border_width(btn_gamepad, 0, 0);
    lv_obj_set_style_pad_hor(btn_gamepad, 0, 0);
    lv_obj_set_style_pad_ver(btn_gamepad, 2, 0);
    lv_obj_add_event_cb(btn_gamepad, home_btn_gamepad_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t * label_gamepad = lv_label_create(btn_gamepad);
    lv_label_set_text(label_gamepad, "gamepad");
    lv_obj_center(label_gamepad);

    lv_obj_t * btn_voltage = lv_button_create(home_scr_cont);
    lv_obj_set_style_border_width(btn_voltage, 0, 0);
    lv_obj_set_style_pad_hor(btn_voltage, 0, 0);
    lv_obj_set_style_pad_ver(btn_voltage, 2, 0);
    lv_obj_add_event_cb(btn_voltage, home_btn_voltage_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t * label_voltage = lv_label_create(btn_voltage);
    lv_label_set_text(label_voltage, "voltage");
    lv_obj_center(label_voltage);
    
    /* 加载页面 */
    lv_screen_load(home_scr);
    
    /* 设置group */
    home_group = lv_group_create();
    lv_group_add_obj(home_group, btn_temp);
    lv_group_add_obj(home_group, btn_humidity);
    lv_group_add_obj(home_group, btn_fan);
    lv_group_add_obj(home_group, btn_file);
    lv_group_add_obj(home_group, btn_time);
    lv_group_add_obj(home_group, btn_gamepad);    
    lv_group_add_obj(home_group, btn_voltage);
    lv_indev_set_group(indev_encoder, home_group); 
    lv_group_set_wrap(home_group, false);
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
    /* 切换到home_scr页面 */
    lv_screen_load(home_scr);
    lv_indev_set_group(indev_encoder, home_group);
    
    /* 删除temp_ui页面和temp_group组 */
    lv_obj_delete_async(temp_scr);
    lv_group_del(temp_group);

    // /* 切换到home_group组 */
    // lv_indev_set_group(indev_encoder, home_group);
}

void temp_ui(void)
{
    /* 创建页面 */
    temp_scr = lv_obj_create(NULL);

    temp_label = lv_label_create(temp_scr);
    lv_obj_center(temp_label);
    lv_label_set_text(temp_label, "T: --.-°C");
    lv_obj_add_event_cb(temp_label, temp_label_event_cb, LV_EVENT_REFRESH, NULL);

    temp_btn_back = lv_button_create(temp_scr);
    lv_obj_add_event_cb(temp_btn_back, temp_btn_back_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_align(temp_btn_back, LV_ALIGN_BOTTOM_LEFT, 3, -3);
    lv_obj_set_size(temp_btn_back, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_style_border_width(temp_btn_back, 0, 0);
    lv_obj_set_style_pad_all(temp_btn_back, 0, 0);
    lv_obj_t * label_back = lv_label_create(temp_btn_back);
    lv_label_set_text(label_back, LV_SYMBOL_LEFT);

    /* 加载页面 */
    lv_screen_load(temp_scr);

    /* 设置group */
    temp_group = lv_group_create();
    lv_group_add_obj(temp_group, temp_btn_back);
    lv_group_set_wrap(temp_group, false);
    lv_indev_set_group(indev_encoder, temp_group);
    lv_group_focus_obj(temp_btn_back);
}

/*====================== humidity ui ======================*/

static void humidity_label_event_cb(lv_event_t * e)
{
    lv_obj_t * label = lv_event_get_target_obj(e);
    lv_label_set_text_fmt(label, "H: %.1f%%", humidity * 100); 
}

static void humidity_btn_back_event_cb(lv_event_t * e)
{
    /* 切换到home_scr页面 */
    lv_screen_load(home_scr);
    lv_indev_set_group(indev_encoder, home_group);

    /* 删除humidity_ui页面和humidity_group组 */
    lv_group_del(humidity_group);
    lv_obj_delete_async(humidity_scr);

    // /* 切换到home_group组 */
    // lv_indev_set_group(indev_encoder, home_group);
}

void humidity_ui(void)
{
    /* 创建页面 */
    humidity_scr = lv_obj_create(NULL);

    humidity_label = lv_label_create(humidity_scr);
    lv_obj_center(humidity_label);
    lv_label_set_text(humidity_label, "H: --.-%");
    lv_obj_add_event_cb(humidity_label, humidity_label_event_cb, LV_EVENT_REFRESH, NULL);

    humidity_btn_back = lv_button_create(humidity_scr);
    lv_obj_add_event_cb(humidity_btn_back, humidity_btn_back_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_align(humidity_btn_back, LV_ALIGN_BOTTOM_LEFT, 3, -3);
    lv_obj_set_size(humidity_btn_back, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_style_border_width(humidity_btn_back, 0, 0);
    lv_obj_set_style_pad_all(humidity_btn_back, 0, 0);
    lv_obj_t * label_back = lv_label_create(humidity_btn_back);
    lv_label_set_text(label_back, LV_SYMBOL_LEFT);

    /* 加载页面 */
    lv_screen_load(humidity_scr);

    /* 设置group */
    humidity_group = lv_group_create();
    lv_group_add_obj(humidity_group, humidity_btn_back);
    lv_group_set_wrap(humidity_group, false);
    lv_indev_set_group(indev_encoder, humidity_group);
    lv_group_focus_obj(humidity_btn_back);
}

/*====================== fan ui ======================*/

static void fan_slider_event_cb(lv_event_t * e)
{
    fan_speed = lv_slider_get_value(lv_event_get_target_obj(e));
    fan_set_speed(fan_speed);
    lv_label_set_text_fmt(fan_label_speed_value, "%d", fan_speed);
}

static void fan_btn_back_event_cb(lv_event_t * e)
{
    /* 切换到home_scr页面 */
    lv_screen_load(home_scr);
    lv_indev_set_group(indev_encoder, home_group);

    /* 删除fan_ui页面和fan_group组 */
    lv_group_del(fan_group);
    lv_obj_delete_async(fan_scr);

    // /* 切换到home_group组 */
    // lv_indev_set_group(indev_encoder, home_group);
}

void fan_ui(void) 
{
    /* 创建页面 */
    fan_scr = lv_obj_create(NULL);

    lv_obj_t * slider_fan_speed = lv_slider_create(fan_scr);
    lv_obj_set_size(slider_fan_speed, LV_PCT(60), LV_PCT(30));
    lv_obj_align(slider_fan_speed, LV_ALIGN_BOTTOM_RIGHT, -13, -5);
    lv_obj_set_style_radius(slider_fan_speed, 3, LV_PART_INDICATOR);
    lv_obj_set_style_radius(slider_fan_speed, 3, LV_PART_KNOB);
    lv_obj_set_style_bg_color(slider_fan_speed, lv_color_hex(0xFFFFFF), LV_PART_KNOB);
    lv_obj_set_style_bg_opa(slider_fan_speed, LV_OPA_COVER, LV_PART_KNOB);
    lv_slider_set_range(slider_fan_speed, -5, 5);
    lv_slider_set_value(slider_fan_speed, fan_speed, LV_ANIM_OFF);
    lv_obj_add_event_cb(slider_fan_speed, fan_slider_event_cb, LV_EVENT_VALUE_CHANGED, NULL);

    lv_obj_t * label_speed = lv_label_create(fan_scr);
    lv_label_set_text(label_speed, "speed");
    lv_obj_align_to(label_speed, slider_fan_speed, LV_ALIGN_OUT_TOP_MID, 0, -5);

    fan_label_speed_value = lv_label_create(fan_scr);
    lv_label_set_text_fmt(fan_label_speed_value, "%d", fan_speed);
    lv_obj_align(fan_label_speed_value, LV_ALIGN_TOP_LEFT, 3, 3);

    fan_btn_back = lv_button_create(fan_scr);
    lv_obj_add_event_cb(fan_btn_back, fan_btn_back_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_align(fan_btn_back, LV_ALIGN_BOTTOM_LEFT, 3, -3);
    lv_obj_set_size(fan_btn_back, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_style_border_width(fan_btn_back, 0, 0);
    lv_obj_set_style_pad_all(fan_btn_back, 0, 0);
    lv_obj_t * label_back = lv_label_create(fan_btn_back);
    lv_label_set_text(label_back, LV_SYMBOL_LEFT);

    /* 加载页面 */
    lv_screen_load(fan_scr);

    /* 设置group */
    fan_group = lv_group_create();
    lv_group_add_obj(fan_group, fan_btn_back);
    lv_group_add_obj(fan_group, slider_fan_speed);
    lv_group_set_wrap(fan_group, false);
    lv_indev_set_group(indev_encoder, fan_group);
    lv_group_focus_obj(fan_btn_back);
}

/*=====================================================================================
================================= file ui begin =======================================
=====================================================================================*/

static void file_file_explorer_file_table_value_changed_event_cb(lv_event_t * e)
{
    uint32_t selected_row, selected_col;
    const char * selected_cell_value;
    lv_file_explorer_file_table_entry_data_t * file_table_entry_user_data;

    lv_obj_t * obj = lv_event_get_target(e); //file_table对象

    // 获取选中的file_table单元格值
    lv_table_get_selected_cell(obj, &selected_row, &selected_col);
    selected_cell_value = lv_table_get_cell_value(obj, selected_row, selected_col);
    strcpy(file_file_table_previous_selected_cell_value, selected_cell_value);

    // 获取选中的file_table单元格用户数据
    file_table_entry_user_data = lv_table_get_cell_user_data(obj, selected_row, selected_col);
    file_file_table_previous_entry_user_data.file_kind = file_table_entry_user_data->file_kind;

    // 获取路径
    strcpy(file_previous_path, lv_file_explorer_get_current_path(file_explorer));
}

static void file_file_explorer_file_table_clicked_event_cb(lv_event_t * e)
{
    lv_fs_file_t f;
    lv_fs_res_t res;
    uint32_t read_num;
    uint8_t buf[128];
    const char * cur_path;
    const char * sel_fn;
    char full_path[LV_FILE_EXPLORER_PATH_MAX_LEN];

    lv_obj_t * obj = lv_event_get_target(e); //file_table对象

    if(file_file_table_previous_entry_user_data.file_kind == LV_FILE_EXPLORER_FILE_KIND_FILE) { // 如果选中的是文件
        //拼接完整路径
        cur_path = lv_file_explorer_get_current_path(file_explorer);
        sel_fn = lv_file_explorer_get_selected_file_name(file_explorer);
        snprintf(full_path, sizeof(full_path), "%s%s", cur_path, sel_fn);
        if(strstr(sel_fn, ".txt") != NULL) { //如果选中的是以.TXT结尾的文件
            //打开txt文件弹窗
            txt_popup_ui(); 
        
            //打开txt文件
            res = lv_fs_open(&f, full_path, LV_FS_MODE_RD);
            if(res != LV_FS_RES_OK) printf("Failed to open file. res=%d\n", res);

            //读取txt文件内容到buf
            res = lv_fs_read(&f, buf, sizeof(buf) - 1, &read_num);
            if(res != LV_FS_RES_OK) printf("Failed to read file. res=%d\n", res);

            //确保字符串以null结尾
            buf[read_num] = '\0';

            //设置txtarea内容为buf
            lv_textarea_set_text(txt_popup_ta, (char*)buf);

            //关闭txt文件
            lv_fs_close(&f);
        } else if (strstr(sel_fn, ".bin") != NULL) { //如果选中的是以.BIN结尾的文件
            //打开bin文件弹窗
            bin_popup_ui();

            //设置image源为bin文件路径
            lv_img_set_src(bin_popup_img_placeholder, full_path);
        }
    } else if (file_file_table_previous_entry_user_data.file_kind == LV_FILE_EXPLORER_FILE_KIND_DIR && // 如果选中的是目录并且是< back并且当前路径是F:0:/
               strcmp((char *)file_file_table_previous_selected_cell_value, LV_SYMBOL_LEFT "  " "Back") == 0 && 
               strcmp(file_previous_path, "F:0:/") == 0 ) {
        /* 切换到home_scr页面 */
        lv_screen_load(home_scr);
        lv_indev_set_group(indev_encoder, home_group);

        /* 删除file_ui页面和file_group组 */
        lv_group_delete(file_group);
        lv_obj_delete_async(file_scr);

        // /* 切换到home_group组 */
        // lv_indev_set_group(indev_encoder, home_group);
    }
}

void file_ui(void)
{
    /* 创建页面 */
    file_scr = lv_obj_create(NULL);

    lv_obj_t * file_scr_cont = lv_obj_create(file_scr);
    lv_obj_set_size(file_scr_cont, LV_PCT(100), LV_PCT(100));
    lv_obj_set_pos(file_scr_cont, 0, 0);
    lv_obj_set_style_border_width(file_scr_cont, 0, 0);
    lv_obj_set_flex_flow(file_scr_cont, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(file_scr_cont, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_END, LV_FLEX_ALIGN_END);

    file_explorer = lv_file_explorer_create(file_scr_cont);
    lv_obj_set_size(file_explorer, LV_PCT(100), LV_PCT(100));
    lv_file_explorer_set_sort(file_explorer, LV_EXPLORER_SORT_KIND);
    lv_file_explorer_open_dir(file_explorer, "F:0:/");

    //获取头部
    lv_obj_t * header = lv_file_explorer_get_header(file_explorer);
    lv_obj_set_size(header, LV_PCT(100), LV_PCT(30));
    lv_obj_set_style_text_font(header, &lv_font_montserrat_16, 0);

    //获取文件表
    lv_obj_t * file_table   = lv_file_explorer_get_file_table(file_explorer);
    lv_obj_set_size(file_table, LV_PCT(100), LV_PCT(70));
    lv_obj_set_style_text_font(file_table, &lv_font_montserrat_16, 0);
    lv_obj_add_event_cb(file_table, file_file_explorer_file_table_value_changed_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_add_event_cb(file_table, file_file_explorer_file_table_clicked_event_cb, LV_EVENT_CLICKED, NULL);

    /* 加载页面 */
    lv_screen_load(file_scr);

    /* 设置group */
    file_group = lv_group_create();
    lv_group_add_obj(file_group, file_table);
    lv_group_set_wrap(file_group, false);
    lv_indev_set_group(indev_encoder, file_group); 
}

/*====================== txt file popup ui ======================*/

static void file_file_explorer_txt_file_pupup_btn_back_event_cb(lv_event_t *e)
{
    /* 删除页面 */
    lv_obj_t *root = lv_obj_get_parent(lv_event_get_target(e));
    lv_obj_delete_async(root);

    /* 切换到file_group组 */
    lv_group_del(txt_popup_group);
    lv_indev_set_group(indev_encoder, file_group);
}

void txt_popup_ui(void)
{
    /* 创建页面 */
    lv_obj_t *container = lv_obj_create(lv_scr_act());
    lv_obj_set_size(container, LV_PCT(85), LV_PCT(85));
    lv_obj_align(container, LV_ALIGN_CENTER, 0, 0);

    txt_popup_ta = lv_textarea_create(container);
    lv_obj_set_size(txt_popup_ta, LV_PCT(70), LV_PCT(100));
    lv_obj_align(txt_popup_ta, LV_ALIGN_RIGHT_MID, 0, 0);
    lv_obj_set_style_border_width(txt_popup_ta, 0, 0);
    lv_textarea_set_placeholder_text(txt_popup_ta, "");     // 清空提示文本

    lv_obj_t *btn_back = lv_button_create(container);
    lv_obj_set_size(btn_back, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_style_border_width(btn_back, 0, 0);
    lv_obj_set_style_pad_all(btn_back, 0, 0);
    lv_obj_align(btn_back, LV_ALIGN_BOTTOM_LEFT, 0, 0);
    lv_obj_t * label_back = lv_label_create(btn_back);
    lv_label_set_text(label_back, LV_SYMBOL_LEFT);
    lv_obj_add_event_cb(btn_back, file_file_explorer_txt_file_pupup_btn_back_event_cb, LV_EVENT_CLICKED, NULL);

    /* 设置group */
    txt_popup_group = lv_group_create();
    lv_group_add_obj(txt_popup_group, btn_back);
    lv_group_add_obj(txt_popup_group, txt_popup_ta);
    lv_group_set_wrap(txt_popup_group, false);
    lv_indev_set_group(indev_encoder, txt_popup_group);
    lv_group_focus_obj(btn_back);
}

/*====================== bin file popup ui ======================*/

static void file_file_explorer_bin_file_pupup_btn_back_event_cb(lv_event_t *e)
{
    /* 删除页面 */
    lv_obj_t *root = lv_obj_get_parent(lv_event_get_target(e));
    lv_obj_delete_async(root);

    /* 切换到file_group组 */
    lv_group_del(bin_popup_group);
    lv_indev_set_group(indev_encoder, file_group);
}

void bin_popup_ui(void)
{
    /* 创建页面 */
    lv_obj_t *container = lv_obj_create(lv_scr_act());
    lv_obj_set_size(container, LV_PCT(85), LV_PCT(85));
    lv_obj_align(container, LV_ALIGN_CENTER, 0, 0);

    bin_popup_img_placeholder = lv_image_create(container);
    lv_image_set_src(bin_popup_img_placeholder, LV_SYMBOL_IMAGE);
    lv_obj_align(bin_popup_img_placeholder, LV_ALIGN_RIGHT_MID, 0, 0);

    lv_obj_t *btn_back = lv_button_create(container);
    lv_obj_set_size(btn_back, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_style_border_width(btn_back, 0, 0);
    lv_obj_set_style_pad_all(btn_back, 0, 0);
    lv_obj_align(btn_back, LV_ALIGN_BOTTOM_LEFT, 0, 0);
    lv_obj_t * label_back = lv_label_create(btn_back);
    lv_label_set_text(label_back, LV_SYMBOL_LEFT);
    lv_obj_add_event_cb(btn_back, file_file_explorer_bin_file_pupup_btn_back_event_cb, LV_EVENT_CLICKED, NULL);

    /* 设置group */
    bin_popup_group = lv_group_create();
    lv_group_add_obj(bin_popup_group, btn_back);
    lv_group_add_obj(bin_popup_group, bin_popup_img_placeholder);
    lv_group_set_wrap(bin_popup_group, false);
    lv_indev_set_group(indev_encoder, bin_popup_group);
    lv_group_focus_obj(btn_back);
}
/*=====================================================================================
================================= file ui end =========================================
=====================================================================================*/

/*====================== time ui ======================*/

static void time_time_label_current_date_event_cb(lv_event_t * e)
{
    lv_obj_t * label = lv_event_get_target_obj(e);
    lv_label_set_text_fmt(label, "%04d-%02d-%02d", calendar.year, calendar.month, calendar.date);
}

static void time_time_label_current_time_event_cb(lv_event_t * e)
{
    lv_obj_t * label = lv_event_get_target_obj(e);
    lv_label_set_text_fmt(label, "%02d:%02d:%02d", calendar.hour, calendar.min, calendar.sec);
}

static void time_btn_back_event_cb(lv_event_t * e)
{
    /* 切换到home_scr页面 */
    lv_screen_load(home_scr);
    lv_indev_set_group(indev_encoder, home_group);

    /* 删除time_ui页面和time_group组 */
    lv_group_del(time_group);
    lv_obj_delete_async(time_scr);

    // /* 切换到home_group组 */
    // lv_indev_set_group(indev_encoder, home_group);
}

void time_ui(void)
{
    /* 创建页面 */
    time_scr = lv_obj_create(NULL);

    time_label_current_date = lv_label_create(time_scr);
    lv_obj_align(time_label_current_date, LV_ALIGN_CENTER, 0, -10);
    lv_label_set_text(time_label_current_date, "0000-00-00");
    lv_obj_add_event_cb(time_label_current_date, time_time_label_current_date_event_cb, LV_EVENT_REFRESH, NULL);

    time_label_current_time = lv_label_create(time_scr);
    lv_obj_align(time_label_current_time, LV_ALIGN_CENTER, 0, 10);
    lv_label_set_text(time_label_current_time, "00:00:00");
    lv_obj_add_event_cb(time_label_current_time, time_time_label_current_time_event_cb, LV_EVENT_REFRESH, NULL);

    time_btn_back = lv_button_create(time_scr);
    lv_obj_add_event_cb(time_btn_back, time_btn_back_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_align(time_btn_back, LV_ALIGN_BOTTOM_LEFT, 3, -3);
    lv_obj_set_size(time_btn_back, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_style_border_width(time_btn_back, 0, 0);
    lv_obj_set_style_pad_all(time_btn_back, 0, 0);
    lv_obj_t * label_back = lv_label_create(time_btn_back);
    lv_label_set_text(label_back, LV_SYMBOL_LEFT);

    /* 加载页面 */
    lv_screen_load(time_scr);

    /* 设置group */
    time_group = lv_group_create();
    lv_group_add_obj(time_group, time_btn_back);
    lv_group_set_wrap(time_group, false);
    lv_indev_set_group(indev_encoder, time_group);
    lv_group_focus_obj(time_btn_back);
}

/*====================== gamepad ui ======================*/

static void gamepad_btn_back_event_cb(lv_event_t * e)
{
    /* 切换到home_scr页面 */
    lv_screen_load(home_scr);
    lv_indev_set_group(indev_encoder, home_group);

    /* 删除gamepad_ui页面和gamepad_group组 */
    lv_group_del(gamepad_group);
    lv_obj_delete_async(gamepad_scr);

    // /* 切换到home_group组 */
    // lv_indev_set_group(indev_encoder, home_group);
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
    gamepad_group = lv_group_create();
    lv_group_add_obj(gamepad_group, gamepad_btn_back);
    lv_group_add_obj(gamepad_group, gamepad_btn_up);
    lv_group_add_obj(gamepad_group, gamepad_btn_right);
    lv_group_add_obj(gamepad_group, gamepad_btn_down);
    lv_group_add_obj(gamepad_group, gamepad_btn_left);
    lv_group_set_wrap(gamepad_group, false);
    lv_indev_set_group(indev_encoder, gamepad_group);
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
    /* 切换到home_scr页面 */
    lv_screen_load(home_scr);
    lv_indev_set_group(indev_encoder, home_group);

    /* 删除voltage_ui页面和group_voltage组 */
    lv_group_del(voltage_group);
    lv_obj_delete_async(voltage_scr);

    // /* 切换到home_group组 */
    // lv_indev_set_group(indev_encoder, home_group);
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