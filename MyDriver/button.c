#include "button.h"


/**
  * @brief  configure button gpio
  * @param  button: specifies the button to be configured.
  * @retval none
  */
void at32_button_init(void)
{
  gpio_init_type gpio_init_struct;

  /* enable the button clock */
  crm_periph_clock_enable(USER_BUTTON_CRM_CLK, TRUE);

  /* set default parameter */
  gpio_default_para_init(&gpio_init_struct);

  /* configure button pin as input with pull-up/pull-down */
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_mode = GPIO_MODE_INPUT;
  gpio_init_struct.gpio_pins = USER_BUTTON_PIN;
  gpio_init_struct.gpio_pull = GPIO_PULL_DOWN;
  gpio_init(USER_BUTTON_PORT, &gpio_init_struct);
}

/**
  * @brief  returns the selected button state
  * @param  none
  * @retval the button gpio pin value
  */
uint8_t at32_button_state(void)
{
  return gpio_input_data_bit_read(USER_BUTTON_PORT, USER_BUTTON_PIN);
}

/**
  * @brief  returns which button have press down
  * @param  none
  * @retval the button have press down
  */
button_type at32_button_press()
{
  static uint8_t pressed = 1;
  /* get button state in at_start board */
  if((pressed == 1) && (at32_button_state() != RESET))
  {
    /* debounce */
    pressed = 0;
    wk_delay_ms(10);
    if(at32_button_state() != RESET)
      return USER_BUTTON;
  }
  else if(at32_button_state() == RESET)
  {
    pressed = 1;
  }
  return NO_BUTTON;
}
