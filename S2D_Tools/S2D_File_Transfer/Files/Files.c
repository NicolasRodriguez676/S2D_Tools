#include <string.h>

#include "S2D_Files.h"

s2d_file_s s2d_file;

void s2d_init_file(TaskHandle_t* rt_handle)
{
    S2D_FILE_DMA_INIT(s2d_file.buffer);
    S2D_FILE_UART_INIT(S2D_FILE_UART_BAUD_RATE);

    S2D_FILE_OUT_TASK_HANDLE(&s2d_file.handle);

    s2d_file.rt_handle = rt_handle;
    s2d_file.semaphore = xSemaphoreCreateMutex();

    xTaskCreate(files_out, "files_out", configMINIMAL_STACK_SIZE * 4, NULL, tskIDLE_PRIORITY + S2D_FILE_OUT_TASK_PRIORITY, &s2d_file.handle);
}

void s2d_serialize_packet(const s2d_data_packet_s *packet, uint8_t* buffer)
{
    uint32_t length;

    memcpy(buffer, packet->header1, HEADER_LENGTH1 * sizeof(float));
    length = HEADER_LENGTH1 * sizeof(float);

    memcpy(buffer + length, packet->header2, HEADER_LENGTH2 * sizeof(float));
    length += HEADER_LENGTH2 * sizeof(float);

    memcpy(buffer + length, packet->data, DATA_LENGTH * sizeof(uint32_t));
    length += DATA_LENGTH * sizeof(uint32_t);

    memcpy(buffer + length, (uint8_t*)(packet->crc32), sizeof(uint32_t));
}