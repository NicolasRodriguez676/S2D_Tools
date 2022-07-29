#ifndef APP_S2D_TOOLS_S2D_TOOLS_CONFIG_H
#define APP_S2D_TOOLS_S2D_TOOLS_CONFIG_H

#include "dma/dma.h"
#include "uart/uart5.h"

#define S2D_DMA_INIT(b1, b2)        init_dma1(b1, b2)
#define S2D_DMA_START(len)          start_dma1(len)

#define S2D_UART_INIT(br)           init_uart5(br)

#define S2D_GIVE_OUT_TASK_HANDLE(h) get_task_handle(h)
#define S2D_OUT_BUFFER_SIZE         256

#endif //APP_S2D_TOOLS_S2D_TOOLS_CONFIG_H
