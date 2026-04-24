#include "bsp_oled.h"

#include "bsp_board.h"
#include "bsp_i2c.h"

#include <string.h>

#define BSP_OLED_PAGE_HEIGHT       ( 8U )
#define BSP_OLED_PAGE_COUNT        ( BSP_OLED_HEIGHT / BSP_OLED_PAGE_HEIGHT )
#define BSP_OLED_FRAMEBUFFER_SIZE  ( BSP_OLED_WIDTH * BSP_OLED_PAGE_COUNT )
#define BSP_OLED_BUS_CHUNK_SIZE    ( 128U )
#define BSP_OLED_I2C_TIMEOUT_MS    ( 100U )

static uint8_t oled_framebuffer[BSP_OLED_FRAMEBUFFER_SIZE];

static bool oled_write_command(uint8_t command);
static bool oled_write_commands(const uint8_t *commands, size_t size);
static void oled_clear_framebuffer(void);
static void oled_draw_pixel(uint8_t x, uint8_t y, bool enabled);

bool bsp_oled_init(void)
{
  static const uint8_t init_sequence[] = {
      0xAEU, /* Display off */
      0x00U, /* Lower column start address */
      0x10U, /* Higher column start address */
      0x40U, /* Display start line */
      0x81U, 0xCFU, /* Contrast */
      0xA1U,        /* Segment remap */
      0xC8U,        /* COM scan direction */
      0xA6U,        /* Normal display */
      0xA8U, 0x3FU, /* Multiplex ratio */
      0xD3U, 0x00U, /* Display offset */
      0xD5U, 0x80U, /* Display clock divide */
      0xD9U, 0xF1U, /* Pre-charge period */
      0xDAU, 0x12U, /* COM pins hardware configuration */
      0xDBU, 0x30U, /* VCOMH deselect level */
      0x20U, 0x02U, /* Page addressing mode */
      0x8DU, 0x14U  /* Charge pump enable */
  };

  HAL_Delay(100U);

  if (!oled_write_commands(init_sequence, sizeof(init_sequence)))
  {
    return false;
  }

  if (!bsp_oled_clear())
  {
    return false;
  }

  return oled_write_command(0xAFU);
}

bool bsp_oled_clear(void)
{
  oled_clear_framebuffer();

  return bsp_oled_flush();
}

bool bsp_oled_flush(void)
{
  for (uint8_t page = 0U; page < BSP_OLED_PAGE_COUNT; page++)
  {
    uint8_t page_commands[] = {
        (uint8_t)(0xB0U | page),
        0x00U,
        0x10U,
    };

    if (!oled_write_commands(page_commands, sizeof(page_commands)))
    {
      return false;
    }

    if (!bsp_oled_bus_write(BSP_OLED_CONTROL_BYTE_DATA,
                            &oled_framebuffer[(size_t)page * BSP_OLED_WIDTH],
                            BSP_OLED_WIDTH))
    {
      return false;
    }
  }

  return true;
}

bool bsp_oled_show_test_pattern(void)
{
  oled_clear_framebuffer();

  for (uint8_t x = 0U; x < BSP_OLED_WIDTH; x++)
  {
    uint8_t y_down = (uint8_t)(((uint16_t)x * (BSP_OLED_HEIGHT - 1U)) /
                              (BSP_OLED_WIDTH - 1U));
    uint8_t y_up = (uint8_t)((BSP_OLED_HEIGHT - 1U) - y_down);

    oled_draw_pixel(x, 0U, true);
    oled_draw_pixel(x, BSP_OLED_HEIGHT - 1U, true);
    oled_draw_pixel(x, y_down, true);
    oled_draw_pixel(x, y_up, true);
  }

  for (uint8_t y = 0U; y < BSP_OLED_HEIGHT; y++)
  {
    oled_draw_pixel(0U, y, true);
    oled_draw_pixel(BSP_OLED_WIDTH - 1U, y, true);
  }

  for (uint8_t y = 8U; y < 24U; y++)
  {
    for (uint8_t x = 8U; x < 32U; x++)
    {
      oled_draw_pixel(x, y, ((x + y) & 1U) == 0U);
    }
  }

  for (uint8_t y = 40U; y < 56U; y++)
  {
    for (uint8_t x = 96U; x < 120U; x++)
    {
      oled_draw_pixel(x, y, true);
    }
  }

  return bsp_oled_flush();
}

bool bsp_oled_bus_write(uint8_t control_byte, const uint8_t *payload, size_t size)
{
  uint8_t tx_buffer[1U + BSP_OLED_BUS_CHUNK_SIZE];
  size_t offset = 0U;

  if ((payload == NULL) && (size > 0U))
  {
    return false;
  }

  while (offset < size)
  {
    size_t chunk_size = size - offset;

    if (chunk_size > BSP_OLED_BUS_CHUNK_SIZE)
    {
      chunk_size = BSP_OLED_BUS_CHUNK_SIZE;
    }

    tx_buffer[0] = control_byte;
    memcpy(&tx_buffer[1], &payload[offset], chunk_size);

    if (!bsp_i2c1_write(BSP_OLED_I2C_ADDRESS_7BIT,
                        tx_buffer,
                        chunk_size + 1U,
                        BSP_OLED_I2C_TIMEOUT_MS))
    {
      return false;
    }

    offset += chunk_size;
  }

  return true;
}

static bool oled_write_command(uint8_t command)
{
  return oled_write_commands(&command, 1U);
}

static bool oled_write_commands(const uint8_t *commands, size_t size)
{
  return bsp_oled_bus_write(BSP_OLED_CONTROL_BYTE_CMD, commands, size);
}

static void oled_clear_framebuffer(void)
{
  memset(oled_framebuffer, 0, sizeof(oled_framebuffer));
}

static void oled_draw_pixel(uint8_t x, uint8_t y, bool enabled)
{
  size_t byte_index;
  uint8_t bit_mask;

  if ((x >= BSP_OLED_WIDTH) || (y >= BSP_OLED_HEIGHT))
  {
    return;
  }

  byte_index = ((size_t)y / BSP_OLED_PAGE_HEIGHT) * BSP_OLED_WIDTH + x;
  bit_mask = (uint8_t)(1U << (y % BSP_OLED_PAGE_HEIGHT));

  if (enabled)
  {
    oled_framebuffer[byte_index] |= bit_mask;
  }
  else
  {
    oled_framebuffer[byte_index] &= (uint8_t)~bit_mask;
  }
}
