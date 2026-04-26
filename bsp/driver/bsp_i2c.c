#include "bsp_i2c.h"

#include "bsp_board.h"

#define BSP_I2C1_TIMING_1MHZ_SYSCLK170 ( 0x1090162AU )

static I2C_HandleTypeDef i2c1_handle;

void bsp_i2c_init(void)
{
  i2c1_handle.Instance = I2C1;
  i2c1_handle.Init.Timing = BSP_I2C1_TIMING_1MHZ_SYSCLK170;
  i2c1_handle.Init.OwnAddress1 = 0U;
  i2c1_handle.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  i2c1_handle.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  i2c1_handle.Init.OwnAddress2 = 0U;
  i2c1_handle.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  i2c1_handle.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  i2c1_handle.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;

  if (HAL_I2C_Init(&i2c1_handle) != HAL_OK)
  {
    Error_Handler();
  }

  if (HAL_I2CEx_ConfigAnalogFilter(&i2c1_handle, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  if (HAL_I2CEx_ConfigDigitalFilter(&i2c1_handle, 0U) != HAL_OK)
  {
    Error_Handler();
  }
}

bool bsp_i2c1_write(uint8_t address_7bit,
                    const uint8_t *data,
                    size_t size,
                    uint32_t timeout_ms)
{
  if ((data == NULL) && (size > 0U))
  {
    return false;
  }

  if (size > UINT16_MAX)
  {
    return false;
  }

  if (size == 0U)
  {
    return true;
  }

  return HAL_I2C_Master_Transmit(&i2c1_handle,
                                 (uint16_t)(address_7bit << 1U),
                                 (uint8_t *)data,
                                 (uint16_t)size,
                                 timeout_ms) == HAL_OK;
}
