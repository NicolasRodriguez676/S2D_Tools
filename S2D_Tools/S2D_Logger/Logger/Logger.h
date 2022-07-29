#ifndef S2D_TOOLS_LOGGER_H
#define S2D_TOOLS_LOGGER_H

#include <stdint.h>

void s2d_init_log();
void s2d_set_log_level(uint32_t level);
void s2d_log(uint32_t level, ...);

#endif // S2D_TOOLS_LOGGER_H
