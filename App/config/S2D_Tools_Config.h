#ifndef APP_S2D_TOOLS_S2D_TOOLS_CONFIG_H
#define APP_S2D_TOOLS_S2D_TOOLS_CONFIG_H

#include <stm32f4xx_ll_usart.h>
#include <stm32f4xx_ll_dma.h>

#include "dma/dma.h"
#include "uart/uart5.h"
#include "uart/uart3.h"
#include "timer/timer2.h"


// LOGGER *****************************************************************
#define S2D_LOG_DMA_INIT(b1, b2)    init_dma1s7(b1, b2)
#define S2D_LOG_DMA_START(len)      start_dma1s7(len)

#define S2D_LOG_UART_INIT(br)       init_uart5(br)

#define S2D_TIMER_INIT()            init_timer2()
#define S2D_GET_COUNT()             LL_TIM_GetCounter(TIM2)
#define S2D_ENABLE_COUNT()          LL_TIM_EnableCounter(TIM2)
#define S2D_DISABLE_COUNT()         LL_TIM_DisableCounter(TIM2)
#define S2D_RESET_COUNT()           LL_TIM_WriteReg(TIM2, CNT, 0)

#define S2D_LOG_OUT_TASK_HANDLE(h)  get_log_handle(h)

#define S2D_LOG_OUT_TASK_WAIT 100

// FILE TRANSFER **********************************************************
#define S2D_FILE_DMA_INIT(b)        init_dma1s1(); init_dma1s3(b)
#define S2D_FILE_DMA_START_TX(len)  start_dma1s3(len)
#define S2D_FILE_DMA_START_RX(len)  start_dma1s1(len)
#define S2D_FILE_DMA_STOP_RX()      stop_dma1s1()

#define S2D_FILE_DMA_SET_BUF_TX(b)  set_dma_tx_buf(b)
#define S2D_FILE_DMA_SET_BUF_RX(b)  set_dma_rx_buf(b)

#define S2D_FILE_UART_INIT(br)      init_uart3(br)
#define S2D_FILE_UART_TX_DATA(d)    LL_USART_TransmitData8(USART3, d)

#define S2D_FILE_UART_DMA           0
#define S2D_FILE_UART_IRQ           1

#define S2D_FILE_UART_RX_IN_DEV(d)  set_uart_input_dev(d)

#define S2D_FILE_OUT_TASK_HANDLE(h) get_file_handle(h)

#endif //APP_S2D_TOOLS_S2D_TOOLS_CONFIG_H
