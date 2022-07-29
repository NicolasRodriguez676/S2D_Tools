#ifndef APP_DMA_H
#define APP_DMA_H

void init_dma1(uint8_t* buffer_one, uint8_t* buffer_two);
void start_dma1(uint32_t buffer_length);
void get_task_handle(TaskHandle_t* handle);

#endif //APP_DMA_H
