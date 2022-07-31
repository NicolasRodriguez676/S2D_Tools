#ifndef APP_S2D_TOOLS_S2D_TOOLS_CONFIG_H
#define APP_S2D_TOOLS_S2D_TOOLS_CONFIG_H

#include "dma/dma.h"
#include "uart/uart5.h"
#include "timer/timer2.h"

#define S2D_DMA_INIT(b1, b2)        init_dma1(b1, b2)
#define S2D_DMA_START(len)          start_dma1(len)

#define S2D_UART_INIT(br)           init_uart5(br)

#define S2D_TIMER_INIT()            init_timer2()
#define S2D_GET_COUNT()             LL_TIM_GetCounter(TIM2)
#define S2D_ENABLE_COUNT()          LL_TIM_EnableCounter(TIM2)
#define S2D_DISABLE_COUNT()         LL_TIM_DisableCounter(TIM2)
#define S2D_RESET_COUNT()           LL_TIM_WriteReg(TIM2, CNT, 0)

#define S2D_GIVE_OUT_TASK_HANDLE(h) get_task_handle(h)

#endif //APP_S2D_TOOLS_S2D_TOOLS_CONFIG_H
