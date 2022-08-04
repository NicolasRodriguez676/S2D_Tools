#include <stdint.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>

#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>

#include "S2D_Log.h"

#include "printf/printf.h"

enum S2D_Buffer_Len_Min
{
    // number of bytes for newline + ansi color = 8
    // s2d tag = 5
    BUF_MIN_LEN = 13
};

// Public Internal Definitions
s2d_out_s s2dout;

// Private Internal Definitions
static int32_t g_s2d_log_level_internal;
static const char* g_s2d_log_level_color[5] = { "\n\x1b[31m", "\n\x1b[33m", "\n\x1b[35m", "\n\x1b[32m", "\n\x1b[36m" };

void s2d_init_log()
{
    s2dout.buffer = s2dout.buffer_one;
    s2dout.length = 0;
    s2dout.buffer_select = S2D_BUFFER_ONE;
    s2dout.semaphore = xSemaphoreCreateMutex();

    g_s2d_log_level_internal = S2D_LOG_DEBUG;

    S2D_LOG_DMA_INIT(s2dout.buffer_one, s2dout.buffer_two);
    S2D_LOG_UART_INIT(S2D_LOG_UART_BAUD_RATE);

    S2D_TIMER_INIT();

    xTaskCreate(logs_out, "logs_out", configMINIMAL_STACK_SIZE * 4, NULL, tskIDLE_PRIORITY + S2D_LOG_OUT_TASK_PRIORITY, &s2dout.log_out_handle);
    S2D_LOG_OUT_TASK_HANDLE(&s2dout.log_out_handle);
}

void s2d_set_log_level(int32_t level)
{
    xSemaphoreTake(s2dout.semaphore, portMAX_DELAY);
    g_s2d_log_level_internal = level;
    xSemaphoreGive(s2dout.semaphore);
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
        case S2D_LOG_INFO:
            if ((g_s2d_log_level_internal >= S2D_LOG_INFO) == true)
                level_denied = false;

            break;

        case S2D_LOG_DEBUG:
            if ((g_s2d_log_level_internal >= S2D_LOG_DEBUG) == true)
                level_denied = false;

            break;

        case S2D_LOG_TIME:
            if ((g_s2d_log_level_internal >= S2D_LOG_TIME) == true)
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

        uint32_t bytes_in_temp_buf = vsnprintf((char *) s2dout.buffer_temp, S2D_LOG_BUFFER_TEMP_SIZE, fmt, args);
        va_end(args);

        if (((int32_t)bytes_in_temp_buf < 0) || (bytes_in_temp_buf >= S2D_LOG_BUFFER_TEMP_SIZE))
        {
            const char* msg = "%sS2D: Log message longer than temp buffer or invalid snprintf input.";
            s2dout.length = snprintf((char *) s2dout.buffer, S2D_LOG_HALF_BUFFER_SIZE - s2dout.length, msg, g_s2d_log_level_color[S2D_LOG_ERROR]);

            xTaskNotifyGive(s2dout.log_out_handle);
            xSemaphoreGive(s2dout.semaphore);

            return;
        }

        if ((bytes_in_temp_buf + BUF_MIN_LEN) <= (S2D_LOG_HALF_BUFFER_SIZE - s2dout.length))
        {
            s2dout.length += snprintf((char *) s2dout.buffer + s2dout.length, S2D_LOG_HALF_BUFFER_SIZE - s2dout.length, "%sS2D: ", g_s2d_log_level_color[level]);

            memcpy(s2dout.buffer + s2dout.length, s2dout.buffer_temp, bytes_in_temp_buf);

            s2dout.length += bytes_in_temp_buf;

            if (s2dout.length >= (S2D_LOG_HALF_BUFFER_SIZE - (S2D_LOG_BUFFER_TEMP_SIZE + BUF_MIN_LEN)))
                xTaskNotifyGive(s2dout.log_out_handle);
        }
        else
        {
            // TODO: Determine which log messages were missed
            xTaskNotifyGive(s2dout.log_out_handle);
        }
    }

    xSemaphoreGive(s2dout.semaphore);
}
