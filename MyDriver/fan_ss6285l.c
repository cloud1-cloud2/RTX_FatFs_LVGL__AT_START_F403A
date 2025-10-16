#include "fan_ss6285l.h"
#include "RTE_Components.h"
#include CMSIS_device_header

uint8_t fan_mode = 0; //0:正转 1:反转
uint8_t fan_speed_grade[5] = {45, 55, 70, 85, 100}; //PWM周期设为100个时钟，这里存放比较寄存器的值

/* 函数中的一些配置不影响结果，比如tmr_output_struct.oc_idle_state的设置 */
void fan_ss6285l_init()
{
	gpio_init_type gpio_init_struct;
  tmr_output_config_type tmr_output_struct;
  tmr_brkdt_config_type tmr_brkdt_struct;

	crm_periph_clock_enable(CRM_IOMUX_PERIPH_CLOCK, TRUE);
	crm_periph_clock_enable(CRM_GPIOE_PERIPH_CLOCK, TRUE);
	crm_periph_clock_enable(CRM_TMR1_PERIPH_CLOCK, TRUE);

  gpio_default_para_init(&gpio_init_struct);

  /* configure the CH1C pin */
  gpio_init_struct.gpio_pins = GPIO_PINS_8;
  gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
  gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_MODERATE;
  gpio_init(GPIOE, &gpio_init_struct);

  /* configure the CH1 pin */
  gpio_init_struct.gpio_pins = GPIO_PINS_9;
  gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
  gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_MODERATE;
  gpio_init(GPIOE, &gpio_init_struct);

  /* GPIO PIN remap */
  gpio_pin_remap_config(TMR1_GMUX_0011, TRUE); 

  /* configure counter settings */
  tmr_cnt_dir_set(TMR1, TMR_COUNT_UP);
  tmr_clock_source_div_set(TMR1, TMR_CLOCK_DIV1);
  tmr_repetition_counter_set(TMR1, 0);
  tmr_period_buffer_enable(TMR1, FALSE);
  tmr_base_init(TMR1, 99, 99);

  /* configure primary mode settings */
  tmr_sub_sync_mode_set(TMR1, FALSE);
  tmr_primary_mode_select(TMR1, TMR_PRIMARY_SEL_RESET);

  /* configure channel 1 output settings */
  tmr_output_struct.oc_mode = TMR_OUTPUT_CONTROL_PWM_MODE_A;
  tmr_output_struct.oc_output_state = FALSE;  
  tmr_output_struct.occ_output_state = FALSE; 
  tmr_output_struct.oc_polarity = TMR_OUTPUT_ACTIVE_HIGH;
  tmr_output_struct.occ_polarity = TMR_OUTPUT_ACTIVE_HIGH;
  tmr_output_struct.oc_idle_state = FALSE;   
  tmr_output_struct.occ_idle_state = FALSE;   
  tmr_output_channel_config(TMR1, TMR_SELECT_CHANNEL_1, &tmr_output_struct);
  tmr_channel_value_set(TMR1, TMR_SELECT_CHANNEL_1, 0);
  tmr_output_channel_buffer_enable(TMR1, TMR_SELECT_CHANNEL_1, FALSE);
  tmr_output_channel_immediately_set(TMR1, TMR_SELECT_CHANNEL_1, FALSE);

  /* configure break and dead-time settings */
  tmr_brkdt_struct.brk_enable = FALSE;
  tmr_brkdt_struct.auto_output_enable = FALSE;
  tmr_brkdt_struct.brk_polarity = TMR_BRK_INPUT_ACTIVE_LOW;
  tmr_brkdt_struct.fcsoen_state = FALSE;
  tmr_brkdt_struct.fcsodis_state = FALSE;
  tmr_brkdt_struct.wp_level = TMR_WP_OFF;
  tmr_brkdt_struct.deadtime = 0;
  tmr_brkdt_config(TMR1, &tmr_brkdt_struct);

  tmr_output_enable(TMR1, TRUE);
  tmr_counter_enable(TMR1, TRUE);
	
	fan_set_speed(0);
}

/* val表示挡位，范围：-5~5 */
void fan_set_speed(int16_t val)
{
    if(val > 0) //前进加停止
    {
        //FI为PWM，BI为低电平
        tmr_channel_enable(TMR1, TMR_SELECT_CHANNEL_1, TRUE);
        tmr_channel_enable(TMR1, TMR_SELECT_CHANNEL_1C, FALSE);
        tmr_channel_value_set(TMR1, TMR_SELECT_CHANNEL_1, fan_speed_grade[val - 1]);
        fan_mode = 0;
    }
    else if(val < 0) //后退加停止
    {
        //FI为低电平，BI为PWM
        tmr_channel_enable(TMR1, TMR_SELECT_CHANNEL_1, FALSE);
        tmr_channel_enable(TMR1, TMR_SELECT_CHANNEL_1C, TRUE);
        tmr_channel_value_set(TMR1, TMR_SELECT_CHANNEL_1, fan_speed_grade[-val - 1]);
        fan_mode = 1;
    }else //停止
    {
        if(fan_mode == 0)
        {
          tmr_channel_enable(TMR1, TMR_SELECT_CHANNEL_1, TRUE);
          tmr_channel_enable(TMR1, TMR_SELECT_CHANNEL_1C, FALSE);
          tmr_channel_value_set(TMR1, TMR_SELECT_CHANNEL_1, 0);
        }else{
          tmr_channel_enable(TMR1, TMR_SELECT_CHANNEL_1, FALSE);
          tmr_channel_enable(TMR1, TMR_SELECT_CHANNEL_1C, TRUE);
          tmr_channel_value_set(TMR1, TMR_SELECT_CHANNEL_1, 0);
        }
    }
}

/* NOTE:
    1、AT32的TIMER1只开启一个通道时，要把这个通道看成没有互补通道的存在，也就是说
      tmr_channel_enable(TMR1, TMR_SELECT_CHANNEL_1, FALSE);
      tmr_channel_enable(TMR1, TMR_SELECT_CHANNEL_1C, TRUE);
      tmr_channel_value_set(TMR1, TMR_SELECT_CHANNEL_1, fan_speed_grade[-val - 1]);
    在上述代码中设置占空比时要写成
      tmr_channel_value_set(TMR1, TMR_SELECT_CHANNEL_1, fan_speed_grade[-val - 1]);
    而不是
      tmr_channel_value_set(TMR1, TMR_SELECT_CHANNEL_1C, fan_speed_grade[-val - 1]);
    另外也要注意开启两个通道和开启单通道时tmr_output_struct.occ_polarity和tmr_output_struct.oc_polarity的设置
    建议更改不同的参数并用逻辑分析仪观察输出波形

    2、失能通道时，输出会变成低电平
*/