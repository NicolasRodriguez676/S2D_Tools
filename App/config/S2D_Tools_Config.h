#ifndef APP_S2D_TOOLS_S2D_TOOLS_CONFIG_H
#define APP_S2D_TOOLS_S2D_TOOLS_CONFIG_H

#include "dma/dma.h"
#include "uart/uart5.h"
#include "uart/uart3.h"
#include "timer/timer2.h"


// LOGGER *****************************************************************

// CHIP SPECIFIC FUNCTIONS: APP -> S2D TOOLS

#define S2D_LOG_DMA_INIT(b1, b2)    init_dma1s7(b1, b2)
#define S2D_LOG_DMA_START(len)      start_dma1s7(len)

#define S2D_LOG_UART_INIT(br)       init_uart5(br)

#define S2D_TIMER_INIT()            init_timer2()
#define S2D_GET_COUNT()             get_counter_value()
#define S2D_ENABLE_COUNT()          enable_counter()
#define S2D_DISABLE_COUNT()         disable_counter()
#define S2D_RESET_COUNT()           set_counter_zero()

// OVERRIDE LOGGER INTERNAL VALUES

#define S2D_LOG_OUT_TASK_WAIT 100


// FILE TRANSFER **********************************************************

// CHIP SPECIFIC FUNCTIONS: APP -> S2D TOOLS

#define S2D_FILE_DMA_INIT_TX()       init_dma1s3()
#define S2D_FILE_DMA_START_TX(len)   start_dma1s3(len)
#define S2D_FILE_DMA_SET_BUF_TX(b)   set_dma_tx_buf(b)

#define S2D_FILE_DMA_INIT_RX()       init_dma1s1()
#define S2D_FILE_DMA_START_RX(len)   start_dma1s1(len)
#define S2D_FILE_DMA_STOP_RX()       stop_dma1s1()
#define S2D_FILE_DMA_SET_BUF_RX(b)   set_dma_rx_buf(b)

#define S2D_FILE_UART_INIT(br)       init_uart3(br)
#define S2D_FILE_UART_TX_DATA(d)     send_byte(d)

#define S2D_FILE_UART_RX_IN_DEV(d)   set_uart_input_dev(d)

// OVERRIDE FILE TRANSFER INTERNAL VALUES


#endif //APP_S2D_TOOLS_S2D_TOOLS_CONFIG_H
