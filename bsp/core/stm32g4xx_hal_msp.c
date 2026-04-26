#include "bsp_board.h"

void HAL_MspInit(void)
{
  __HAL_RCC_SYSCFG_CLK_ENABLE();
  __HAL_RCC_PWR_CLK_ENABLE();
}

void HAL_MspDeInit(void)
{
}

void HAL_I2C_MspInit(I2C_HandleTypeDef *hi2c)
{
  if (hi2c->Instance == I2C1)
  {
    GPIO_InitTypeDef gpio_init = {0};
    RCC_PeriphCLKInitTypeDef periph_clk_init = {0};

    periph_clk_init.PeriphClockSelection = RCC_PERIPHCLK_I2C1;
    periph_clk_init.I2c1ClockSelection = RCC_I2C1CLKSOURCE_SYSCLK;

    if (HAL_RCCEx_PeriphCLKConfig(&periph_clk_init) != HAL_OK)
    {
      Error_Handler();
    }

    if (__HAL_RCC_GET_I2C1_SOURCE() != RCC_I2C1CLKSOURCE_SYSCLK)
    {
      Error_Handler();
    }

    HAL_I2CEx_EnableFastModePlus(I2C_FASTMODEPLUS_I2C1);

    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_I2C1_CLK_ENABLE();

    gpio_init.Pin = GPIO_PIN_7 | GPIO_PIN_8;
    gpio_init.Mode = GPIO_MODE_AF_OD;
    gpio_init.Pull = GPIO_PULLUP;
    gpio_init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    gpio_init.Alternate = GPIO_AF4_I2C1;
    HAL_GPIO_Init(GPIOB, &gpio_init);
  }
}

void HAL_I2C_MspDeInit(I2C_HandleTypeDef *hi2c)
{
  if (hi2c->Instance == I2C1)
  {
    __HAL_RCC_I2C1_CLK_DISABLE();

    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_7 | GPIO_PIN_8);
  }
}
