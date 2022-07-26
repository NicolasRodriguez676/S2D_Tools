#include <FreeRTOS.h>
#include <task.h>

#include <stm32f4xx_ll_bus.h>
#include <stm32f4xx_ll_gpio.h>
#include <stm32f4xx_ll_dma.h>
#include <stm32f4xx_ll_usart.h>

#include "S2D_Tools/S2D_Files.h"


TaskHandle_t* s2d_file_handle;

void init_uart3(uint32_t baud_rate)
{
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART3);
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOC);

    /**USART3 GPIO Configuration
    PC10   ------> USART3_TX
    PC11   ------> USART3_RX
    */

    LL_GPIO_InitTypeDef GPIO_InitStruct = { 0 };

    GPIO_InitStruct.Pin         = LL_GPIO_PIN_10 | LL_GPIO_PIN_11;
    GPIO_InitStruct.Mode        = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed       = LL_GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.OutputType  = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull        = LL_GPIO_PULL_NO;
    GPIO_InitStruct.Alternate   = LL_GPIO_AF_7;
    LL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    // RX
    LL_DMA_SetChannelSelection(DMA1, LL_DMA_STREAM_1, LL_DMA_CHANNEL_4);
    LL_DMA_SetDataTransferDirection(DMA1, LL_DMA_STREAM_1, LL_DMA_DIRECTION_PERIPH_TO_MEMORY);
    LL_DMA_SetStreamPriorityLevel(DMA1, LL_DMA_STREAM_1, LL_DMA_PRIORITY_HIGH);
    LL_DMA_SetMode(DMA1, LL_DMA_STREAM_1, LL_DMA_MODE_NORMAL);
    LL_DMA_SetPeriphIncMode(DMA1, LL_DMA_STREAM_1, LL_DMA_PERIPH_NOINCREMENT);
    LL_DMA_SetMemoryIncMode(DMA1, LL_DMA_STREAM_1, LL_DMA_MEMORY_INCREMENT);
    LL_DMA_SetPeriphSize(DMA1, LL_DMA_STREAM_1, LL_DMA_PDATAALIGN_BYTE);
    LL_DMA_SetMemorySize(DMA1, LL_DMA_STREAM_1, LL_DMA_MDATAALIGN_BYTE);
    LL_DMA_DisableFifoMode(DMA1, LL_DMA_STREAM_1);

    // TX
    LL_DMA_SetChannelSelection(DMA1, LL_DMA_STREAM_3, LL_DMA_CHANNEL_4);
    LL_DMA_SetDataTransferDirection(DMA1, LL_DMA_STREAM_3, LL_DMA_DIRECTION_MEMORY_TO_PERIPH);
    LL_DMA_SetStreamPriorityLevel(DMA1, LL_DMA_STREAM_3, LL_DMA_PRIORITY_HIGH);
    LL_DMA_SetMode(DMA1, LL_DMA_STREAM_3, LL_DMA_MODE_NORMAL);
    LL_DMA_SetPeriphIncMode(DMA1, LL_DMA_STREAM_3, LL_DMA_PERIPH_NOINCREMENT);
    LL_DMA_SetMemoryIncMode(DMA1, LL_DMA_STREAM_3, LL_DMA_MEMORY_INCREMENT);
    LL_DMA_SetPeriphSize(DMA1, LL_DMA_STREAM_3, LL_DMA_PDATAALIGN_BYTE);
    LL_DMA_SetMemorySize(DMA1, LL_DMA_STREAM_3, LL_DMA_MDATAALIGN_BYTE);
    LL_DMA_DisableFifoMode(DMA1, LL_DMA_STREAM_3);

    NVIC_SetPriority(USART3_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 6, 0));
    NVIC_EnableIRQ(USART3_IRQn);

    LL_USART_InitTypeDef USART_InitStruct = { 0 };

    USART_InitStruct.BaudRate               = baud_rate;
    USART_InitStruct.DataWidth              = LL_USART_DATAWIDTH_8B;
    USART_InitStruct.StopBits               = LL_USART_STOPBITS_1;
    USART_InitStruct.Parity                 = LL_USART_PARITY_NONE;
    USART_InitStruct.TransferDirection      = LL_USART_DIRECTION_TX_RX;
    USART_InitStruct.HardwareFlowControl    = LL_USART_HWCONTROL_NONE;
    USART_InitStruct.OverSampling           = LL_USART_OVERSAMPLING_16;
    LL_USART_Init(USART3, &USART_InitStruct);

    LL_USART_ConfigAsyncMode(USART3);
    LL_USART_EnableDMAReq_TX(USART3);
    LL_USART_Enable(USART3);
}

// TODO: Split into two macros/functions
void set_uart_input_dev(uint32_t dev)
{
    if (dev == S2D_FILE_UART_DMA)
    {
        LL_USART_DisableIT_RXNE(USART3);
        LL_USART_EnableDMAReq_RX(USART3);
    }
    else // if (dev == S2D_FILE_UART_IRQ)
    {
        LL_USART_DisableDMAReq_RX(USART3);
        LL_USART_EnableIT_RXNE(USART3);
    }
}

void send_byte(uint8_t d)
{
    LL_USART_TransmitData8(USART3, d);
}

void USART3_IRQHandler(void)
{
    if (LL_USART_IsActiveFlag_RXNE(USART3) && LL_USART_IsEnabledIT_RXNE(USART3))
    {
        uint8_t rx_data = LL_USART_ReceiveData8(USART3);
        S2D_FILE_UART_NOTIFY_FROM_ISR(rx_data);
    }
}
