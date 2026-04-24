#ifndef BSP_U8G2_PORT_H
#define BSP_U8G2_PORT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "u8x8.h"

#include <stdbool.h>
#include <stdint.h>

uint8_t u8x8_byte_stm32_i2c(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);
uint8_t u8x8_gpio_and_delay_stm32(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);

void bsp_u8g2_port_reset_status(void);
bool bsp_u8g2_port_is_ok(void);

#ifdef __cplusplus
}
#endif

#endif /* BSP_U8G2_PORT_H */
