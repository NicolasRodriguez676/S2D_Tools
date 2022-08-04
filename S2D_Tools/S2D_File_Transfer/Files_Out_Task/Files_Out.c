#include "S2D_Log.h"
#include "S2D_Files.h"

// use free rtos stream buffers for PC start file transfer!

extern s2d_file_s s2d_file;

void files_out()
{
    uint32_t notify_value;
    vTaskSuspend(NULL);

    for (;;)
    {
        // PC start (resume from ISR)
        S2D_DEBUG("Resume from ISR");

        // send ACK to PC
        S2D_DEBUG("files out sending ACK");
        S2D_FILE_UART_TX_DATA(S2D_FILE_ACK_DATA);

        // Notify Registered Task to fill file with data
        S2D_DEBUG("Notify registered task to fill data");
        xTaskNotify(*s2d_file.rt_handle, S2D_FILE_RT_FLAG, eSetBits);

        // Wait for file to be filled
        S2D_DEBUG("Suspending...");
        vTaskSuspend(NULL);

        xSemaphoreTake(s2d_file.semaphore, portMAX_DELAY);
        S2D_DEBUG("Resuming...");

        // serialize data
        S2D_DEBUG("Serialize data");
        //s2d_serialize_packet(&s2d_file.packet, s2d_file.buffer);

        // send data
        S2D_DEBUG("Sending data...");
        //S2D_FILE_DMA_START(PACKET_SIZE);

        // wait for ACK. Minimum time is how long it takes to transmit
        // file over UART at the current baud rate
        xTaskNotifyWait(UINT32_MAX, 0, &notify_value, pdMS_TO_TICKS(S2D_FILE_OUT_TASK_WAIT));

        // if NACK (timeout/error), send data once more
        if ((notify_value & S2D_FILE_ACK_FLAG) != S2D_FILE_ACK_FLAG)
        {
            S2D_DEBUG("Timeout or NACK received. Try one more time");
            // send data once more
        }

        S2D_DEBUG("File transfer complete. Suspending...");

        // stop
        xSemaphoreGive(s2d_file.semaphore);

        // Wait for next PC start
        vTaskSuspend(NULL);
    }
}
