#ifndef S2D_TOOLS_S2D_LOG_H
#define S2D_TOOLS_S2D_LOG_H

#include <stdbool.h>

#include <FreeRTOS.h>
#include <semphr.h>

#include "S2D_Tools_Config.h"
#include "S2D_Logger/Log_Out_Task/Logs_Out.h"
#include "S2D_Logger/Logger/Logger.h"

#ifndef S2D_GIVE_OUT_TASK_HANDLE
#error "Error: Missing GIVE TASK HANDLE definition."
#endif

#if !defined S2D_DMA_INIT && !defined S2D_DMA_START
#error "Error: Missing DMA init and start defintions."
#endif

#ifndef S2D_UART_INIT
#error "Error: Missing UART init definition."
#endif

#ifndef S2D_DISABLE_TIME_LOGGING
#if !defined S2D_TIMER_INIT
#error "Error: Missing TIMER init defintion."
#endif

#if !defined S2D_GET_COUNT && !defined S2D_RESET_COUNT
#error "Error: Missing GET COUNT and RESET COUNT defintions."
#endif

#if !defined S2D_ENABLE_COUNT && !defined S2D_DISABLE_COUNT
#error "Error: Missing ENABLE COUNT and DISABLE COUNT defintions."
#endif
#endif

#define S2D_LOG_NONE     -1
#define S2D_LOG_INFO      4
#define S2D_LOG_DEBUG     3
#define S2D_LOG_TIME      2
#define S2D_LOG_WARN      1
#define S2D_LOG_ERROR     0

#ifndef S2D_TASK_OUT_WAIT
#define S2D_TASK_OUT_WAIT 1000
#endif

#ifndef S2D_TASK_PRIORITY
#define S2D_TASK_PRIORITY 10
#endif

#ifndef S2D_NOTIFY_DMA_BUSY_FLAG
#define S2D_NOTIFY_DMA_BUSY_FLAG 0x80000000
#endif

#ifndef S2D_OUT_BUFFER_SIZE
#define S2D_OUT_BUFFER_SIZE 1024
#endif

#ifdef S2D_OUT_HALF_BUFFER_SIZE
#error "Error: Half buffer size defined internally!"
#else
#define S2D_OUT_HALF_BUFFER_SIZE (S2D_OUT_BUFFER_SIZE / 2)
#endif

#ifndef S2D_OUT_BUFFER_TEMP_SIZE
#define S2D_OUT_BUFFER_TEMP_SIZE 192
#endif

#ifndef S2D_UART_BAUD_RATE
#define S2D_UART_BAUD_RATE  115200
#endif

#ifdef S2D_DMA_BUSY_WAIT
#error "Error: Busy wait time calculated internally!"
#else
#define S2D_DMA_BUSY_WAIT (uint32_t)(S2D_OUT_HALF_BUFFER_SIZE)*((1/(float)S2D_UART_BAUD_RATE)*(10))
#endif

#ifdef S2D_LOG_DISABLED
#define S2D_SET_LEVEL_INFO()        {}
#define S2D_SET_LEVEL_DEBUG()       {}
#define S2D_SET_LEVEL_WARN()        {}
#define S2D_SET_LEVEL_ERROR()       {}
#define S2D_LOG_INIT()              {}
#define S2D_INFO(...)               {}
#define S2D_DEBUG(...)              {}
#define S2D_WARN(...)               {}
#define S2D_ERROR(...)              {}
#else
#define S2D_SET_LEVEL_INFO()   s2d_set_log_level(S2D_LOG_INFO)
#define S2D_SET_LEVEL_DEBUG()  s2d_set_log_level(S2D_LOG_DEBUG)
#define S2D_SET_LEVEL_WARN()   s2d_set_log_level(S2D_LOG_WARN)
#define S2D_SET_LEVEL_ERROR()  s2d_set_log_level(S2D_LOG_ERROR)
#define S2D_LOG_INIT()         s2d_init_log()
#define S2D_INFO(...)          s2d_log(S2D_LOG_INFO,  __VA_ARGS__)
#define S2D_DEBUG(...)         s2d_log(S2D_LOG_DEBUG, __VA_ARGS__)
#define S2D_WARN(...)          s2d_log(S2D_LOG_WARN,  __VA_ARGS__)
#define S2D_ERROR(...)         s2d_log(S2D_LOG_ERROR, __VA_ARGS__)
#endif

#if defined S2D_LOG_DISABLED || defined S2D_DISABLE_TIME_LOGGING
#define S2D_TIME_START()            {}
#define S2D_TIME_STOP(...)          {}
#else
// S2D time resolution assumed 100ns
#define S2D_TIME_START()   S2D_ENABLE_COUNT()
#define S2D_TIME_STOP()    do                                                                           \
                           {                                                                            \
                               S2D_DISABLE_COUNT();                                                     \
                               s2d_log(S2D_LOG_TIME, "time=%.1fus", ((float)(S2D_GET_COUNT())*(1E-1))); \
                               S2D_RESET_COUNT();                                                       \
                           } while (false)
#endif

enum S2D_Buffer_Names
{
    S2D_BUFFER_ONE = 0,
    S2D_BUFFER_TWO = 1
};

typedef struct S2D_Buffer_Out {
    uint8_t* buffer;
    uint32_t length;

    uint32_t buffer_select;
    uint8_t buffer_one[S2D_OUT_HALF_BUFFER_SIZE];
    uint8_t buffer_two[S2D_OUT_HALF_BUFFER_SIZE];

    uint8_t buffer_temp[S2D_OUT_BUFFER_TEMP_SIZE];

    TaskHandle_t log_out_handle;
    SemaphoreHandle_t semaphore;

} s2d_out_s;

#endif //S2D_TOOLS_S2D_LOG_H
