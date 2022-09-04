#include <FreeRTOS.h>
#include <task.h>

#include <stm32f4xx_ll_bus.h>
#include <stm32f4xx_ll_dma.h>

#include "S2D_Log.h"
#include "S2D_Files.h"

#include "dma.h"


// DMA1 STREAM1 (FILE_RX) *******************************************************************

void init_dma1s1()
{
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);

    NVIC_SetPriority(DMA1_Stream1_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 8, 0));
    NVIC_EnableIRQ(DMA1_Stream1_IRQn);

    LL_DMA_SetPeriphAddress(DMA1, LL_DMA_STREAM_1, (uint32_t)&(USART3->DR));
    LL_DMA_SetDataTransferDirection(DMA1, LL_DMA_STREAM_1, LL_DMA_DIRECTION_PERIPH_TO_MEMORY);
    LL_DMA_EnableIT_TC(DMA1, LL_DMA_STREAM_1);
}

void start_dma1s1(uint32_t buffer_length)
{
    LL_DMA_SetDataLength(DMA1, LL_DMA_STREAM_1, buffer_length);
    LL_DMA_EnableStream(DMA1, LL_DMA_STREAM_1);
}

void stop_dma1s1()
{
    LL_DMA_DisableStream(DMA1, LL_DMA_STREAM_1);
}

void DMA1_Stream1_IRQHandler()
{
    LL_DMA_ClearFlag_TC1(DMA1);
    LL_DMA_DisableStream(DMA1, LL_DMA_STREAM_1);

    S2D_FILE_DMA_NOTIFY_FROM_ISR();
}

// DMA1 STREAM3 (FILE_TX) *******************************************************************

void init_dma1s3()
{
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);

    NVIC_SetPriority(DMA1_Stream3_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 7, 0));
    NVIC_EnableIRQ(DMA1_Stream3_IRQn);

    LL_DMA_SetPeriphAddress(DMA1, LL_DMA_STREAM_3, (uint32_t)&(USART3->DR));
    LL_DMA_SetDataTransferDirection(DMA1, LL_DMA_STREAM_3, LL_DMA_DIRECTION_MEMORY_TO_PERIPH);
    LL_DMA_EnableIT_TC(DMA1, LL_DMA_STREAM_3);
}

void start_dma1s3(uint32_t buffer_size)
{
    LL_DMA_SetDataLength(DMA1, LL_DMA_STREAM_3, buffer_size);
    LL_DMA_EnableStream(DMA1, LL_DMA_STREAM_3);
}

void DMA1_Stream3_IRQHandler()
{
    LL_DMA_ClearFlag_TC3(DMA1);
    LL_DMA_DisableStream(DMA1, LL_DMA_STREAM_3);
}

// helper functions

void set_dma_tx_buf(uint8_t* buffer)
{
    LL_DMA_SetMemoryAddress(DMA1, LL_DMA_STREAM_3, (uint32_t)buffer);
}

void set_dma_rx_buf(uint8_t* buffer)
{
    LL_DMA_SetMemoryAddress(DMA1, LL_DMA_STREAM_1, (uint32_t)buffer);
}

// DMA1 STREAM7 (LOGGER) *****************************************************************

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

void start_dma1s7(uint32_t buffer_length)
{
    LL_DMA_SetDataLength(DMA1, LL_DMA_STREAM_7, buffer_length);
    LL_DMA_EnableStream(DMA1, LL_DMA_STREAM_7);
}

void DMA1_Stream7_IRQHandler()
{
    LL_DMA_ClearFlag_TC7(DMA1);
    LL_DMA_DisableStream(DMA1, LL_DMA_STREAM_7);

    S2D_LOG_DMA_NOTIFY_FROM_ISR();
}
