#ifndef MODULE_DISPLAY_H
#define MODULE_DISPLAY_H

#ifdef __cplusplus
extern "C" {
#endif

#include "u8g2.h"

#include <stdbool.h>

bool module_display_init(void);
bool module_display_show_demo(void);
u8g2_t *module_display_get_u8g2(void);

#ifdef __cplusplus
}
#endif

#endif /* MODULE_DISPLAY_H */
