#include <FreeRTOS.h>
#include <task.h>

#include "S2D_Log.h"

extern s2d_out_s s2d_out;


// DMA is used in double buffered mode

// Task to swap buffers and initiate DMA writing to UART
// S2D_LOG_OUT_TASK_WAIT -> wait for possible logs in selected data_buffer.
// can be ended early if selected data_buffer is almost full
// check if DMA is still busy from previous s2d_start
// take control of s2d_out
// if DMA is busy, wait maximum amount of time (data_buffer size * (1/baud rate) * 10)
// s2d_start DMA and set it to busy
// swap buffers
// release semaphore

void logs_out()
{
    uint32_t notify_value;
    bool dma_busy = true;

    for (;;)
    {
        xTaskNotifyWait(UINT32_MAX, 0, &notify_value, pdMS_TO_TICKS(S2D_LOG_OUT_TASK_WAIT));

        if (notify_value & S2D_LOG_NOTIFY_DMA_BUSY_FLAG)
        {
            dma_busy = false;
            continue;
        }

        xSemaphoreTake(s2d_out.semaphore, portMAX_DELAY);

        if (s2d_out.length > 0)
        {
            if (dma_busy == true)
                xTaskNotifyWait(0, 0, &notify_value, pdMS_TO_TICKS(S2D_LOG_DMA_BUSY_WAIT));

            S2D_LOG_DMA_START(s2d_out.length);
            dma_busy = true;

            if (s2d_out.buffer_select == S2D_BUFFER_TWO)
            {
                s2d_out.buffer = s2d_out.buffer_one;
                s2d_out.buffer_select = S2D_BUFFER_ONE;
            }
            else
            {
                s2d_out.buffer = s2d_out.buffer_two;
                s2d_out.buffer_select = S2D_BUFFER_TWO;
            }

            s2d_out.length = 0;
        }

        xSemaphoreGive(s2d_out.semaphore);
    }
}
