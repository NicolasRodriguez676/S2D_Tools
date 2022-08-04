#ifndef APP_DMA_H
#define APP_DMA_H

void init_dma1s7(uint8_t* buffer_one, uint8_t* buffer_two);
void start_dma1s7(uint32_t buffer_length);
void get_log_handle(TaskHandle_t* handle);

void init_dma1s3(uint8_t* buffer);
void start_dma1s3(uint32_t buffer_length);

#endif //APP_DMA_H
