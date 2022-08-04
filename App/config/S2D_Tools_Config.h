#ifndef APP_S2D_TOOLS_S2D_TOOLS_CONFIG_H
#define APP_S2D_TOOLS_S2D_TOOLS_CONFIG_H

#include <stm32f4xx_ll_usart.h>

#include "dma/dma.h"
#include "uart/uart5.h"
#include "uart/uart3.h"
#include "timer/timer2.h"

#define S2D_LOG_DMA_INIT(b1, b2)    init_dma1s7(b1, b2)
#define S2D_LOG_DMA_START(len)      start_dma1s7(len)

#define S2D_LOG_UART_INIT(br)       init_uart5(br)

#define S2D_TIMER_INIT()            init_timer2()
#define S2D_GET_COUNT()             LL_TIM_GetCounter(TIM2)
#define S2D_ENABLE_COUNT()          LL_TIM_EnableCounter(TIM2)
#define S2D_DISABLE_COUNT()         LL_TIM_DisableCounter(TIM2)
#define S2D_RESET_COUNT()           LL_TIM_WriteReg(TIM2, CNT, 0)

#define S2D_LOG_OUT_TASK_HANDLE(h)  get_log_handle(h)


#define S2D_FILE_DMA_INIT(b)        init_dma1s3(b)
#define S2D_FILE_DMA_START(len)     start_dma1s3(len)

#define S2D_FILE_UART_INIT(br)      init_uart3(br)
#define S2D_FILE_UART_TX_DATA(d)    LL_USART_TransmitData8(USART3, d)

#define S2D_FILE_OUT_TASK_HANDLE(h) get_file_handle(h)

#endif //APP_S2D_TOOLS_S2D_TOOLS_CONFIG_H
