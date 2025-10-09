#include "usbdevice.h"

usbd_core_type usb_core_dev;
gamepad_report1_t report1 ={
	.report_id = 0x01,
	.left_x = 0x7FFF, .left_y = 0x7FFF,
	.right_z = 0x7FFF, .right_rz = 0x7FFF,
  .left_trigger = 0, .right_trigger = 0,
  .dpad = 0, .buttons = 0, .share_button = 0
};
gamepad_report2_t report2 = {
  .report_id = 0x02,
  .enable = 0, .magnitude = {0, 0, 0, 0},
  .duration = 0, .start_delay = 0, .loop_count = 0
};

void usbdevice_init()
{
	/* select usb 48m clcok source */
	usb_clock48m_select(USB_CLK_HICK);
	
	/* enable usb clock */
  crm_periph_clock_enable(CRM_USB_PERIPH_CLOCK, TRUE);
	
	/* enable usb interrupt */
  nvic_irq_enable(USBFS_L_CAN1_RX0_IRQn, 5, 0);

  /* usb core init */
  usbd_core_init(&usb_core_dev, USB, &custom_hid_class_handler, &custom_hid_desc_handler, 0);

  /* enable usb pull-up */
  usbd_connect(&usb_core_dev);
}

/**
  * @brief  usb 48M clock select
  * @param  clk_s:USB_CLK_HICK, USB_CLK_HEXT
  * @retval none
  */
void usb_clock48m_select(usb_clk48_s clk_s)
{
  if(clk_s == USB_CLK_HICK)
  {
    crm_usb_clock_source_select(CRM_USB_CLOCK_SOURCE_HICK);

    /* enable the acc calibration ready interrupt */
    crm_periph_clock_enable(CRM_ACC_PERIPH_CLOCK, TRUE);

    /* update the c1\c2\c3 value */
    acc_write_c1(7980);
    acc_write_c2(8000);
    acc_write_c3(8020);

    /* open acc calibration */
    acc_calibration_mode_enable(ACC_CAL_HICKTRIM, TRUE);
  }
  else
  {
    switch(system_core_clock)
    {
      /* 48MHz */
      case 48000000:
        crm_usb_clock_div_set(CRM_USB_DIV_1);
        break;

      /* 72MHz */
      case 72000000:
        crm_usb_clock_div_set(CRM_USB_DIV_1_5);
        break;

      /* 96MHz */
      case 96000000:
        crm_usb_clock_div_set(CRM_USB_DIV_2);
        break;

      /* 120MHz */
      case 120000000:
        crm_usb_clock_div_set(CRM_USB_DIV_2_5);
        break;

      /* 144MHz */
      case 144000000:
        crm_usb_clock_div_set(CRM_USB_DIV_3);
        break;

      /* 168MHz */
      case 168000000:
        crm_usb_clock_div_set(CRM_USB_DIV_3_5);
        break;

      /* 192MHz */
      case 192000000:
        crm_usb_clock_div_set(CRM_USB_DIV_4);
        break;

      default:
        break;

    }
  }
}

/**
  * @brief  this function handles usb interrupt.
  * @param  none
  * @retval none
  */
void USBFS_L_CAN1_RX0_IRQHandler(void)
{
  usbd_irq_handler(&usb_core_dev);
}


/**
  * @brief  usb delay millisecond function.
  * @param  ms: number of millisecond delay
  * @retval none
  */
void usb_delay_ms(uint32_t ms)
{
  /* user can define self delay function */
  wk_delay_ms(ms);
}

/**
  * @brief  usb delay microsecond function.
  * @param  us: number of microsecond delay
  * @retval none
  */
void usb_delay_us(uint32_t us)
{
  wk_delay_us(us);
}

