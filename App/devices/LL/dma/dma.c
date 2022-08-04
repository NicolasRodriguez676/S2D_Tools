#include <FreeRTOS.h>
#include <task.h>

#include <stm32f4xx_ll_bus.h>
#include <stm32f4xx_ll_dma.h>

#include "S2D_Log.h"

static TaskHandle_t* s2d_log_handle;

void init_dma1s3(uint8_t* buffer)
{
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);

    NVIC_SetPriority(DMA1_Stream3_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 7, 0));
    NVIC_EnableIRQ(DMA1_Stream3_IRQn);

    LL_DMA_ConfigAddresses(DMA1, LL_DMA_STREAM_3, (uint32_t)&(USART3->DR), (uint32_t)buffer, LL_DMA_DIRECTION_PERIPH_TO_MEMORY);
    LL_DMA_EnableIT_TC(DMA1, LL_DMA_STREAM_3);
}

void init_dma1s7(uint8_t* buffer_one, uint8_t* buffer_two)
{
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);

    NVIC_SetPriority(DMA1_Stream7_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 6, 0));
    NVIC_EnableIRQ(DMA1_Stream7_IRQn);

    LL_DMA_EnableDoubleBufferMode(DMA1, LL_DMA_STREAM_7);

    LL_DMA_ConfigAddresses(DMA1, LL_DMA_STREAM_7, (uint32_t)&(UART5->DR), (uint32_t)buffer_one, LL_DMA_DIRECTION_PERIPH_TO_MEMORY);
    LL_DMA_SetMemory1Address(DMA1, LL_DMA_STREAM_7, (uint32_t)buffer_two);
    LL_DMA_EnableIT_TC(DMA1, LL_DMA_STREAM_7);
}

void start_dma1s3(uint32_t buffer_length)
{
    LL_DMA_SetDataLength(DMA1, LL_DMA_STREAM_3, buffer_length);
    LL_DMA_EnableStream(DMA1, LL_DMA_STREAM_3);
}

void start_dma1s7(uint32_t buffer_length)
{
    LL_DMA_SetDataLength(DMA1, LL_DMA_STREAM_7, buffer_length);
    LL_DMA_EnableStream(DMA1, LL_DMA_STREAM_7);
}

void get_log_handle(TaskHandle_t* handle)
{
    s2d_log_handle = handle;
}

void DMA1_Stream3_IRQHandler()
{
    LL_DMA_ClearFlag_TC7(DMA1);
    LL_DMA_DisableStream(DMA1, LL_DMA_STREAM_3);
}

void DMA1_Stream7_IRQHandler()
{
    LL_DMA_ClearFlag_TC7(DMA1);
    LL_DMA_DisableStream(DMA1, LL_DMA_STREAM_7);

    uint32_t prev_notify;
    xTaskGenericNotifyFromISR(*s2d_log_handle, S2D_LOG_NOTIFY_DMA_BUSY_FLAG, eSetBits, &prev_notify, NULL);
}
