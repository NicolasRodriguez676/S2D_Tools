#include <string.h>

#include "S2D_Files.h"
#include "Files.h"
#include "printf/printf.h"


s2d_file_s s2d_file;

void s2d_init_file(TaskHandle_t* rt_handle)
{
    S2D_FILE_DMA_INIT();
    S2D_FILE_DMA_SET_BUF_TX(s2d_file.data_buffer);
    S2D_FILE_DMA_SET_BUF_RX(s2d_file.header_buffer);

    S2D_FILE_UART_INIT(S2D_FILE_UART_BAUD_RATE);
    S2D_FILE_UART_RX_IN_DEV(S2D_FILE_UART_IRQ);

    S2D_FILE_OUT_TASK_HANDLE(&s2d_file.handle);

    s2d_file.rt_handle = rt_handle;
    s2d_file.semaphore = xSemaphoreCreateMutex();

    xTaskCreate(files_out, "files_out", configMINIMAL_STACK_SIZE * 4, NULL, tskIDLE_PRIORITY + S2D_FILE_OUT_TASK_PRIORITY, &s2d_file.handle);

    s2d_file.state = WAITING;
}

void s2d_init_packet(s2d_data_packet_s* packet)
{
    s2d_file.packet = packet;
}

void s2d_serialize_packet(const s2d_data_packet_s* packet, uint8_t* buffer)
{
    memcpy(buffer, packet->data, packet->num_samples * sizeof(float));
}

void s2d_deserialize_header(s2d_data_packet_s *packet, uint8_t* buffer)
{
    memcpy(&packet->num_samples, buffer, sizeof(uint32_t));
    memcpy(&packet->sampling_freq, buffer + sizeof(uint32_t), sizeof(float));
}
