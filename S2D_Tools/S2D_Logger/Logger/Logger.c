#include <stdint.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdatomic.h>

#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <string.h>

#include "S2D_Log.h"

#include "printf/printf.h"

enum S2D_Buffer_Len_Min
{
    // number of bytes for newline\return and color sequences
    // this also includes the buffer select value
    BUF_MIN_LEN = 28
};

// Public Internal Definitions
s2d_out_s s2dout;

// Private Internal Definitions
static atomic_int g_s2d_log_level_internal;
static const char* g_s2d_log_level_color[4] = { "\x1b[31m", "\x1b[33m", "\x1b[36m", "\x1b[32m" };

void s2d_init_log()
{
    s2dout.buffer = s2dout.buffer_one;
    s2dout.length = 0;
    s2dout.buffer_select = S2D_BUFFER_ONE;
    s2dout.semaphore = xSemaphoreCreateMutex();

    g_s2d_log_level_internal = S2D_LOG_DEBUG;

    S2D_DMA_INIT(s2dout.buffer_one, s2dout.buffer_two);
    S2D_UART_INIT(S2D_UART_BAUD_RATE);

    xTaskCreate(logs_out, "logs_out", configMINIMAL_STACK_SIZE * 4, NULL, tskIDLE_PRIORITY + 10, &s2dout.log_out_handle);
    S2D_GIVE_OUT_TASK_HANDLE(&s2dout.log_out_handle);
}

void s2d_set_log_level(uint32_t level)
{
    g_s2d_log_level_internal = level;
}

void s2d_log(uint32_t level, ...)
{
    xSemaphoreTake(s2dout.semaphore, portMAX_DELAY);

    if (g_s2d_log_level_internal == S2D_LOG_NONE)
    {
        xSemaphoreGive(s2dout.semaphore);
        return;
    }

    bool level_denied = true;

    switch(level)
    {
        case S2D_LOG_DEBUG:
            if ((g_s2d_log_level_internal >= S2D_LOG_DEBUG) == true)
                level_denied = false;

            break;

        case S2D_LOG_INFO:
            if ((g_s2d_log_level_internal >= S2D_LOG_INFO) == true)
                level_denied = false;

            break;

        case S2D_LOG_WARN:
            if ((g_s2d_log_level_internal >= S2D_LOG_WARN) == true)
                level_denied = false;

            break;

        case S2D_LOG_ERROR:
            if ((g_s2d_log_level_internal >= S2D_LOG_ERROR) == true)
                level_denied = false;

            break;

        default:
            break;
    }

    if (level_denied == false)
    {
        va_list args;
        va_start(args, level);
        const char *fmt = va_arg(args, char*);

        uint32_t bytes_in_temp_buf;

        bytes_in_temp_buf = vsnprintf((char *) s2dout.buffer_temp, S2D_OUT_HALF_BUFFER_SIZE, fmt, args);

        // check if temp buffer can fit inside selected buffer
        if (bytes_in_temp_buf <= (S2D_OUT_HALF_BUFFER_SIZE - s2dout.length) - BUF_MIN_LEN)
        {
            s2dout.length += snprintf((char *) s2dout.buffer + s2dout.length, S2D_OUT_HALF_BUFFER_SIZE - s2dout.length, "%s[%d] S2D: ", g_s2d_log_level_color[level], s2dout.buffer_select);

            memcpy(s2dout.buffer + s2dout.length, s2dout.buffer_temp, bytes_in_temp_buf);

            s2dout.length += bytes_in_temp_buf;
            s2dout.length += snprintf((char *) s2dout.buffer + s2dout.length, S2D_OUT_HALF_BUFFER_SIZE - s2dout.length, "\x1b[0m\n\r");
        }
        else
        {
            // i would want to use the task to print the data in the temp buffer immediately or lose the data
            // at least i can lose one log by notifying the task that the buffer is full so that the next
            // log will succeed.
            // maybe add something to the next log that will indicate that one was missed.

            xTaskNotifyGive(s2dout.log_out_handle);
        }

        va_end(args);
    }

    xSemaphoreGive(s2dout.semaphore);
}

