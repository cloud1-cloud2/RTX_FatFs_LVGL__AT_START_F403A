#include "encoder_ec11.h"

void encoder_ec11_init()
{
	gpio_init_type gpio_init_struct;
  tmr_input_config_type  tmr_input_struct;

	crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK, TRUE);
	crm_periph_clock_enable(CRM_TMR4_PERIPH_CLOCK, TRUE);
	
  gpio_default_para_init(&gpio_init_struct);

	/* configure the KEY pin */
	gpio_init_struct.gpio_mode = GPIO_MODE_INPUT;
  gpio_init_struct.gpio_pins = GPIO_PINS_5;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init(GPIOB, &gpio_init_struct);
	
  /* configure the A pin */
  gpio_init_struct.gpio_pins = GPIO_PINS_6;
  gpio_init_struct.gpio_mode = GPIO_MODE_INPUT;
  gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init(GPIOB, &gpio_init_struct);

  /* configure the B pin */
  gpio_init_struct.gpio_pins = GPIO_PINS_7;
  gpio_init_struct.gpio_mode = GPIO_MODE_INPUT;
  gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init(GPIOB, &gpio_init_struct);

  /* configure counter settings */
  tmr_cnt_dir_set(TMR4, TMR_COUNT_UP);
  tmr_clock_source_div_set(TMR4, TMR_CLOCK_DIV1);
  tmr_period_buffer_enable(TMR4, FALSE);
  tmr_base_init(TMR4, 65535, 1);

  /* configure primary mode settings */
  tmr_sub_sync_mode_set(TMR4, FALSE);
  tmr_primary_mode_select(TMR4, TMR_PRIMARY_SEL_RESET);

  /* configure encoder mode */
  tmr_input_struct.input_channel_select = TMR_SELECT_CHANNEL_1;
  tmr_input_struct.input_mapped_select = TMR_CC_CHANNEL_MAPPED_DIRECT;
  tmr_input_struct.input_polarity_select = TMR_INPUT_RISING_EDGE;
  tmr_input_struct.input_filter_value = 15;
  tmr_input_channel_init(TMR4, &tmr_input_struct, TMR_CHANNEL_INPUT_DIV_1);

  tmr_input_struct.input_channel_select = TMR_SELECT_CHANNEL_2;
  tmr_input_struct.input_mapped_select = TMR_CC_CHANNEL_MAPPED_DIRECT;
  tmr_input_struct.input_polarity_select = TMR_INPUT_RISING_EDGE;
  tmr_input_struct.input_filter_value = 15;
  tmr_input_channel_init(TMR4, &tmr_input_struct, TMR_CHANNEL_INPUT_DIV_1);

  tmr_encoder_mode_config(TMR4, TMR_ENCODER_MODE_A, TMR_INPUT_RISING_EDGE, TMR_INPUT_RISING_EDGE);

  tmr_counter_enable(TMR4, TRUE);
}

