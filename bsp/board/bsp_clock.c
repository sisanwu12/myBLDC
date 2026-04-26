#include "bsp_clock.h"

#include "bsp_board.h"

#define BSP_CLOCK_SYSCLK_HZ ( 170000000UL )

void bsp_clock_init(void)
{
  RCC_OscInitTypeDef rcc_osc_init = {0};
  RCC_ClkInitTypeDef rcc_clk_init = {0};

  __HAL_RCC_PWR_CLK_ENABLE();

  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1_BOOST) != HAL_OK)
  {
    Error_Handler();
  }

  rcc_osc_init.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  rcc_osc_init.HSIState = RCC_HSI_ON;
  rcc_osc_init.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  rcc_osc_init.PLL.PLLState = RCC_PLL_ON;
  rcc_osc_init.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  rcc_osc_init.PLL.PLLM = RCC_PLLM_DIV4;
  rcc_osc_init.PLL.PLLN = 85U;
  rcc_osc_init.PLL.PLLP = RCC_PLLP_DIV7;
  rcc_osc_init.PLL.PLLQ = RCC_PLLQ_DIV2;
  rcc_osc_init.PLL.PLLR = RCC_PLLR_DIV2;

  if (HAL_RCC_OscConfig(&rcc_osc_init) != HAL_OK)
  {
    Error_Handler();
  }

  rcc_clk_init.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                           RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  rcc_clk_init.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  rcc_clk_init.AHBCLKDivider = RCC_SYSCLK_DIV1;
  rcc_clk_init.APB1CLKDivider = RCC_HCLK_DIV1;
  rcc_clk_init.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&rcc_clk_init, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }

  if ((HAL_RCC_GetSysClockFreq() != BSP_CLOCK_SYSCLK_HZ) ||
      (HAL_RCC_GetHCLKFreq() != BSP_CLOCK_SYSCLK_HZ) ||
      (HAL_RCC_GetPCLK1Freq() != BSP_CLOCK_SYSCLK_HZ) ||
      (HAL_RCC_GetPCLK2Freq() != BSP_CLOCK_SYSCLK_HZ))
  {
    Error_Handler();
  }
}
