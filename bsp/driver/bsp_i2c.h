#ifndef BSP_I2C_H
#define BSP_I2C_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

void bsp_i2c_init(void);
bool bsp_i2c1_write(uint8_t address_7bit,
                    const uint8_t *data,
                    size_t size,
                    uint32_t timeout_ms);

#ifdef __cplusplus
}
#endif

#endif /* BSP_I2C_H */
