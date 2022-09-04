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
    BUF_MIN_LEN = 6
};

// Public Internal Definitions
s2d_out_s s2d_out;

// Private Internal Definitions
static int32_t g_s2d_log_level_internal;
//static const char* g_s2d_log_level_color[5] = { "\x1b[31m", "\x1b[33m", "\x1b[35m", "\x1b[32m", "\x1b[36m" };

static const uint8_t g_s2d_log_level_color[5][5] = {
        {0x1b,0x5b,0x33,0x31,0x6d},
        {0x1b,0x5b,0x33,0x33,0x6d},
        {0x1b,0x5b,0x33,0x35,0x6d},
        {0x1b,0x5b,0x33,0x32,0x6d},
        {0x1b,0x5b,0x33,0x36,0x6d}
};

void s2d_init_log()
{
    s2d_out.buffer = s2d_out.buffer_one;
    s2d_out.length = 0;
    s2d_out.buffer_select = S2D_BUFFER_ONE;
    s2d_out.semaphore = xSemaphoreCreateMutex();

    g_s2d_log_level_internal = S2D_LOG_DEBUG;

    S2D_LOG_DMA_INIT(s2d_out.buffer_one, s2d_out.buffer_two);
    S2D_LOG_UART_INIT(S2D_LOG_UART_BAUD_RATE);

    S2D_TIMER_INIT();

    xTaskCreate(logs_out, "logs_out", configMINIMAL_STACK_SIZE * 4, NULL, tskIDLE_PRIORITY + S2D_LOG_OUT_TASK_PRIORITY, &s2d_out.handle);
}

void s2d_set_log_level(int32_t level)
{
    xSemaphoreTake(s2d_out.semaphore, portMAX_DELAY);
    g_s2d_log_level_internal = level;
    xSemaphoreGive(s2d_out.semaphore);
}

void s2d_log(uint32_t level, ...)
{
    xSemaphoreTake(s2d_out.semaphore, portMAX_DELAY);

    if (g_s2d_log_level_internal == S2D_LOG_NONE)
    {
        xSemaphoreGive(s2d_out.semaphore);
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

        uint32_t bytes_in_temp_buf = vsnprintf((char *) s2d_out.buffer_temp, S2D_LOG_BUFFER_TEMP_SIZE, fmt, args);
        va_end(args);

        if (((int32_t)bytes_in_temp_buf < 0) || (bytes_in_temp_buf >= S2D_LOG_BUFFER_TEMP_SIZE))
        {
            const char* msg = "%sLog message longer than temp data_buffer or invalid snprintf input.\n";
            s2d_out.length = snprintf((char *) s2d_out.buffer, S2D_LOG_HALF_BUFFER_SIZE - s2d_out.length, msg, g_s2d_log_level_color[S2D_LOG_ERROR]);

            xTaskNotifyGive(s2d_out.handle);
            xSemaphoreGive(s2d_out.semaphore);

            return;
        }

        if ((bytes_in_temp_buf + BUF_MIN_LEN) <= (S2D_LOG_HALF_BUFFER_SIZE - s2d_out.length))
        {
            memcpy(s2d_out.buffer + s2d_out.length, g_s2d_log_level_color[level], 5);
            s2d_out.length += 5;

            memcpy(s2d_out.buffer + s2d_out.length, s2d_out.buffer_temp, bytes_in_temp_buf);
            s2d_out.length += bytes_in_temp_buf;

            memcpy(s2d_out.buffer + s2d_out.length, "\n", 1);
            s2d_out.length += 1;

            if (s2d_out.length >= (S2D_LOG_HALF_BUFFER_SIZE - (S2D_LOG_BUFFER_TEMP_SIZE + BUF_MIN_LEN)))
                xTaskNotifyGive(s2d_out.handle);
        }
        else
        {
            // TODO: Determine which log messages were missed
            xTaskNotifyGive(s2d_out.handle);
        }
    }

    xSemaphoreGive(s2d_out.semaphore);
}

void s2d_log_dma_notify_from_isr()
{
    xTaskGenericNotifyFromISR(s2d_out.handle, S2D_LOG_NOTIFY_DMA_BUSY_FLAG, eSetBits, NULL, NULL);
}
