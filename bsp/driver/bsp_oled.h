#ifndef BSP_OLED_H
#define BSP_OLED_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define BSP_OLED_WIDTH              ( 128U )
#define BSP_OLED_HEIGHT             ( 64U )
#define BSP_OLED_I2C_ADDRESS_7BIT   ( 0x3CU )
#define BSP_OLED_CONTROL_BYTE_CMD   ( 0x00U )
#define BSP_OLED_CONTROL_BYTE_DATA  ( 0x40U )

bool bsp_oled_init(void);
bool bsp_oled_clear(void);
bool bsp_oled_flush(void);
bool bsp_oled_show_test_pattern(void);
bool bsp_oled_bus_write(uint8_t control_byte, const uint8_t *payload, size_t size);

#ifdef __cplusplus
}
#endif

#endif /* BSP_OLED_H */
