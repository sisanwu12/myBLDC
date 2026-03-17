#include "bsp_init.h"

#include "bsp_adc.h"
#include "bsp_board.h"
#include "bsp_clock.h"
#include "bsp_gpio.h"
#include "bsp_pwm.h"
#include "bsp_tim.h"
#include "bsp_uart.h"

void bsp_init(void)
{
  HAL_Init();
  bsp_clock_init();
  bsp_gpio_init();
  bsp_uart_init();
  bsp_pwm_init();
  bsp_adc_init();
  bsp_tim_init();
}

void Error_Handler(void)
{
  __disable_irq();

  while (1)
  {
  }
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line)
{
  (void)file;
  (void)line;

  Error_Handler();
}
#endif
