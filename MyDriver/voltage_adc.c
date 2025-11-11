#include "voltage_adc.h"

uint16_t voltage_raw_value;

static void wk_dma_channel_config(dma_channel_type* dmax_channely, uint32_t peripheral_base_addr, uint32_t memory_base_addr, uint16_t buffer_size);

void voltage_adc_init(void)
{
  gpio_init_type gpio_init_struct;
  dma_init_type dma_init_struct;
  adc_base_config_type adc_base_struct;

	/* 外设时钟设置 */
  crm_periph_clock_enable(CRM_GPIOC_PERIPH_CLOCK, TRUE);
  crm_periph_clock_enable(CRM_DMA1_PERIPH_CLOCK, TRUE);
  crm_periph_clock_enable(CRM_ADC1_PERIPH_CLOCK, TRUE);

  /* GPIO设置 */
  gpio_default_para_init(&gpio_init_struct);
  gpio_init_struct.gpio_mode = GPIO_MODE_ANALOG;
  gpio_init_struct.gpio_pins = GPIO_PINS_1;
  gpio_init(GPIOC, &gpio_init_struct);

  /* DMA设置 */
  dma_reset(DMA1_CHANNEL1);
  dma_default_para_init(&dma_init_struct);
  dma_init_struct.direction = DMA_DIR_PERIPHERAL_TO_MEMORY;
  dma_init_struct.memory_data_width = DMA_MEMORY_DATA_WIDTH_HALFWORD;
  dma_init_struct.memory_inc_enable = TRUE;
  dma_init_struct.peripheral_data_width = DMA_PERIPHERAL_DATA_WIDTH_HALFWORD;
  dma_init_struct.peripheral_inc_enable = FALSE;
  dma_init_struct.priority = DMA_PRIORITY_LOW;
  dma_init_struct.loop_mode_enable = TRUE;
  dma_init(DMA1_CHANNEL1, &dma_init_struct);
  dma_flexible_config(DMA1, FLEX_CHANNEL1, DMA_FLEXIBLE_ADC1);
  wk_dma_channel_config(DMA1_CHANNEL1, (uint32_t)&ADC1->odt, (uint32_t)&voltage_raw_value, sizeof(voltage_raw_value)/sizeof(uint16_t));

  /* ADC设置 */
  adc_reset(ADC1);
  crm_adc_clock_div_set(CRM_ADC_DIV_6);
  adc_combine_mode_select(ADC_INDEPENDENT_MODE);
  adc_base_default_para_init(&adc_base_struct);
  adc_base_struct.sequence_mode = FALSE;
  adc_base_struct.repeat_mode = TRUE;
  adc_base_struct.data_align = ADC_RIGHT_ALIGNMENT;
  adc_base_struct.ordinary_channel_length = 1;
  adc_base_config(ADC1, &adc_base_struct);
  adc_ordinary_channel_set(ADC1, ADC_CHANNEL_11, 1, ADC_SAMPLETIME_1_5);
  adc_ordinary_conversion_trigger_set(ADC1, ADC12_ORDINARY_TRIG_SOFTWARE, TRUE);
  adc_ordinary_part_mode_enable(ADC1, FALSE);
  adc_dma_mode_enable(ADC1, TRUE);
  adc_enable(ADC1, TRUE);
  
  /* ADC校准 */
  adc_calibration_init(ADC1);
  while(adc_calibration_init_status_get(ADC1));
  adc_calibration_start(ADC1);
  while(adc_calibration_status_get(ADC1));
	
  /* 使能DMA通道和ADC软件触发 */
  adc_ordinary_software_trigger_enable(ADC1, TRUE);
  dma_channel_enable(DMA1_CHANNEL1, TRUE);
}

/* 12位ADC  VREF+ = VDDA(3.3V)  VREF- = VSSA(0V) */
float voltage_adc_get_voltage_value(void)
{
  return (float)voltage_raw_value * 3.3f / 4095.0f;
}

static void wk_dma_channel_config(dma_channel_type* dmax_channely, uint32_t peripheral_base_addr, uint32_t memory_base_addr, uint16_t buffer_size)
{
  dmax_channely->dtcnt = buffer_size;
  dmax_channely->paddr = peripheral_base_addr;
  dmax_channely->maddr = memory_base_addr;
}