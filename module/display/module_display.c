#include "display/module_display.h"

#include "bsp_u8g2_port.h"

static u8g2_t display_u8g2;
static bool display_initialized = false;

bool module_display_init(void)
{
  bsp_u8g2_port_reset_status();

  u8g2_Setup_ssd1306_i2c_128x64_noname_f(&display_u8g2,
                                          U8G2_R0,
                                          u8x8_byte_stm32_i2c,
                                          u8x8_gpio_and_delay_stm32);
  u8g2_InitDisplay(&display_u8g2);
  u8g2_SetPowerSave(&display_u8g2, 0U);

  display_initialized = bsp_u8g2_port_is_ok();

  return display_initialized;
}

bool module_display_show_demo(void)
{
  if (!display_initialized)
  {
    return false;
  }

  bsp_u8g2_port_reset_status();

  u8g2_ClearBuffer(&display_u8g2);
  u8g2_SetFont(&display_u8g2, u8g2_font_6x10_tf);
  u8g2_SetFontMode(&display_u8g2, 1U);
  u8g2_DrawFrame(&display_u8g2, 0U, 0U, 128U, 64U);
  u8g2_DrawLine(&display_u8g2, 0U, 0U, 127U, 63U);
  u8g2_DrawLine(&display_u8g2, 127U, 0U, 0U, 63U);
  u8g2_DrawBox(&display_u8g2, 6U, 42U, 24U, 14U);
  u8g2_DrawFrame(&display_u8g2, 94U, 42U, 28U, 14U);
  u8g2_DrawStr(&display_u8g2, 28U, 18U, "U8G2 OLED OK");
  u8g2_DrawStr(&display_u8g2, 38U, 34U, "SSD1306 I2C");
  u8g2_SendBuffer(&display_u8g2);

  return bsp_u8g2_port_is_ok();
}

u8g2_t *module_display_get_u8g2(void)
{
  return &display_u8g2;
}
