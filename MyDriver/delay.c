#include "delay.h"

#define STEP_DELAY_MS                    (uint32_t)(50)
#define TICK_COUNT_MAX                   (uint32_t)(0xFFFF)
#define TICK_COUNT_VALUE                 (TMR6->cval)

/* global variable */
volatile uint32_t ticks_count_us;

void delay_init()
{	
	crm_periph_clock_enable(CRM_TMR6_PERIPH_CLOCK, TRUE);
  wk_timebase_init();
}

/**
  * @brief  this function provides minimum delay (in microsecond).
  * @param  delay: specifies the delay time length, in microsecond.
  * @retval none
  */
__WEAK void wk_delay_us(uint32_t delay)
{
  uint32_t delay_ticks, pre_ticks, cur_ticks, delta;
  delay_ticks = delay * ticks_count_us;

  pre_ticks = TICK_COUNT_VALUE;
  do
  {
    cur_ticks = TICK_COUNT_VALUE;
    /* count up */
    delta = (cur_ticks >= pre_ticks) ? (cur_ticks - pre_ticks) : ((TICK_COUNT_MAX - pre_ticks) + cur_ticks + 1);
  } while(delta < delay_ticks);
}

/**
  * @brief  this function provides minimum delay (in milliseconds).
  * @param  delay variable specifies the delay time length, in milliseconds.
  * @retval none
  */
__WEAK void wk_delay_ms(uint32_t delay)
{
  while(delay)
  {
    if(delay > STEP_DELAY_MS)
    {
      wk_delay_us(STEP_DELAY_MS * 1000);
      delay -= STEP_DELAY_MS;
    }
    else
    {
      wk_delay_us(delay * 1000);
      delay = 0;
    }
  }
}

/**
  * @brief  this function configures the source of the time base.
  * @param  none
  * @retval none
  */
__WEAK void wk_timebase_init(void)
{
  crm_clocks_freq_type crm_clocks;
  uint32_t frequency = 0;

  /* get crm_clocks */
  crm_clocks_freq_get(&crm_clocks);

  frequency = crm_clocks.apb1_freq * 2;

  /* time base configuration */
  tmr_base_init(TMR6, TICK_COUNT_MAX, ((frequency / 1000000U) - 1));
  ticks_count_us = 1U;
  /* config timer count up */
  tmr_cnt_dir_set(TMR6, TMR_COUNT_UP);
  /* enable timer */
  tmr_counter_enable(TMR6, TRUE);
}

