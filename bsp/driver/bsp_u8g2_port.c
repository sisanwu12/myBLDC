#include "bsp_u8g2_port.h"

#include "bsp_board.h"
#include "bsp_i2c.h"
#include "bsp_oled.h"

#include <string.h>

#define BSP_U8G2_I2C_TIMEOUT_MS  ( 100U )
#define BSP_U8G2_TX_BUFFER_SIZE  ( 128U )

static uint8_t u8g2_tx_buffer[BSP_U8G2_TX_BUFFER_SIZE];
static size_t u8g2_tx_size = 0U;
static bool u8g2_transfer_ok = true;

uint8_t u8x8_byte_stm32_i2c(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
  switch (msg)
  {
    case U8X8_MSG_BYTE_INIT:
      return 1U;

    case U8X8_MSG_BYTE_START_TRANSFER:
      u8g2_tx_size = 0U;
      u8g2_transfer_ok = true;
      return 1U;

    case U8X8_MSG_BYTE_SEND:
      if ((arg_ptr == NULL) || ((u8g2_tx_size + arg_int) > sizeof(u8g2_tx_buffer)))
      {
        u8g2_transfer_ok = false;
        return 0U;
      }

      memcpy(&u8g2_tx_buffer[u8g2_tx_size], arg_ptr, arg_int);
      u8g2_tx_size += arg_int;
      return 1U;

    case U8X8_MSG_BYTE_END_TRANSFER:
    {
      uint8_t address_7bit = u8x8_GetI2CAddress(u8x8);

      if (!u8g2_transfer_ok)
      {
        return 0U;
      }

      if (u8g2_tx_size == 0U)
      {
        return 1U;
      }

      address_7bit = (address_7bit == 255U) ? BSP_OLED_I2C_ADDRESS_7BIT
                                             : (uint8_t)(address_7bit >> 1U);
      u8g2_transfer_ok = bsp_i2c1_write(address_7bit,
                                        u8g2_tx_buffer,
                                        u8g2_tx_size,
                                        BSP_U8G2_I2C_TIMEOUT_MS);
      return u8g2_transfer_ok ? 1U : 0U;
    }

    case U8X8_MSG_BYTE_SET_DC:
      return 1U;

    default:
      return 0U;
  }
}

uint8_t u8x8_gpio_and_delay_stm32(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
  (void)u8x8;
  (void)arg_ptr;

  switch (msg)
  {
    case U8X8_MSG_GPIO_AND_DELAY_INIT:
      return 1U;

    case U8X8_MSG_DELAY_MILLI:
      HAL_Delay(arg_int);
      return 1U;

    case U8X8_MSG_DELAY_NANO:
    case U8X8_MSG_DELAY_100NANO:
    case U8X8_MSG_DELAY_10MICRO:
    case U8X8_MSG_DELAY_I2C:
      return 1U;

    default:
      u8x8_SetGPIOResult(u8x8, 1U);
      return 1U;
  }
}

void bsp_u8g2_port_reset_status(void)
{
  u8g2_transfer_ok = true;
}

bool bsp_u8g2_port_is_ok(void)
{
  return u8g2_transfer_ok;
}
