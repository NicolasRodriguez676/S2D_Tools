#ifndef APP_DMA_H
#define APP_DMA_H

// DMA1 STREAM3 (FILE) *******************************************************************

void init_dma1s1();
void start_dma1s1(uint32_t buffer_length);
void stop_dma1s1();

void init_dma1s3();
void start_dma1s3(uint32_t buffer_size);

void set_dma_tx_buf(uint8_t* buffer);
void set_dma_rx_buf(uint8_t* buffer);

// DMA1 STREAM7 (LOGGER) *****************************************************************

void init_dma1s7(uint8_t* buffer_one, uint8_t* buffer_two);
void start_dma1s7(uint32_t buffer_length);
void retrieve_log_handle(TaskHandle_t* handle);

#endif //APP_DMA_H
