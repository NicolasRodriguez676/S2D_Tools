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
        xSemaphoreTake(s2d_file.semaphore, portMAX_DELAY);
        s2d_file.state = ACTIVE;

        // PC start (resume from ISR)
        S2D_DEBUG("Resume from ISR");

        // receive waveform parameters. can time-out.
        s2d_file.state = RECEIVING;

        S2D_FILE_UART_RX_IN_DEV(S2D_FILE_UART_DMA);
        S2D_FILE_DMA_START_RX(HEADER_SIZE);

        // send ACK to PC
        S2D_DEBUG("files out sending ACK");

        S2D_FILE_UART_TX_DATA(S2D_FILE_ACK_DATA);
        xTaskNotifyWait(UINT32_MAX, 0, &notify_value, pdMS_TO_TICKS(2500));

        // if NACK (time-out), then terminate
        if ((notify_value & S2D_FILE_ACK_FLAG) != S2D_FILE_ACK_FLAG)
        {
            s2d_file.state = WAITING;

            S2D_FILE_UART_RX_IN_DEV(S2D_FILE_UART_IRQ);

            // TODO: determine if DMA buffer needs to be 'reset'
            S2D_FILE_DMA_STOP_RX();

            xSemaphoreGive(s2d_file.semaphore);

            S2D_WARN("Timeout or NACK received. Stopping");
            goto suspend_task;
        }

        s2d_file.state = ACTIVE;
        S2D_FILE_UART_RX_IN_DEV(S2D_FILE_UART_IRQ);

        // deserialize header information
        s2d_deserialize_header(s2d_file.packet, s2d_file.header_buffer);

        S2D_DEBUG("samples=%d", s2d_file.packet->num_samples);
        S2D_DEBUG("frequency=%.2f", s2d_file.packet->sampling_freq);

//        s2d_file.state = WAITING;
//        xSemaphoreGive(s2d_file.semaphore);
//        S2D_DEBUG("Ending early...");
//        goto suspend_task;

        // Notify Registered Task to fill file with data
        S2D_DEBUG("Notify registered task to fill data");
        xTaskNotify(*s2d_file.worker_handle, S2D_FILE_RT_FLAG, eSetBits);

        // Wait for file to be filled
        S2D_DEBUG("Suspending...");
        vTaskSuspend(NULL);

        S2D_DEBUG("Resuming...");

        // serialize data
        S2D_DEBUG("Serialize data");
        s2d_serialize_packet(s2d_file.packet, s2d_file.data_buffer);

        // send data
        S2D_DEBUG("Sending %d bytes...", s2d_file.packet->num_samples * sizeof(float));
        S2D_FILE_DMA_START_TX(s2d_file.packet->num_samples * sizeof(float));

        // wait for ACK. Minimum time is how long it takes to transmit
        // file over UART at the current baud rate
        // maybe set up a loop waiting for an ACK/NACK and a 'keep alive' byte
        // more time efficient, especially when dealing with large files
        xTaskNotifyWait(UINT32_MAX, 0, &notify_value, pdMS_TO_TICKS(10000));

        // if NACK (timeout/error), send data once more
        if ((notify_value & S2D_FILE_ACK_FLAG) != S2D_FILE_ACK_FLAG)
        {
            S2D_WARN("Timeout or NACK received. Try one more time");
            // send data once more
            S2D_FILE_DMA_START_TX(s2d_file.packet->num_samples);
        }

        S2D_DEBUG("File transfer complete. Suspending...");

        // stop
        s2d_file.state = WAITING;
        xSemaphoreGive(s2d_file.semaphore);

        // Wait for next PC start
        suspend_task:
        vTaskSuspend(NULL);
    }
}
