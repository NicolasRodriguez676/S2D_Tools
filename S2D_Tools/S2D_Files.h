#ifndef APP_S2D_TOOLS_S2D_FILES_H
#define APP_S2D_TOOLS_S2D_FILES_H

#include <FreeRTOS.h>
#include <semphr.h>

#include "S2D_Tools_Config.h"
#include "S2D_File_Transfer/Files_Out_Task/Files_Out.h"
#include "S2D_File_Transfer/Files/Files.h"

#ifndef S2D_FILE_DMA_INIT_TX
#error "[S2D FILE] Missing DMA INIT TX definition."
#endif

#ifndef S2D_FILE_DMA_START_TX
#error "[S2D FILE] Missing DMA START TX definition."
#endif

#ifndef S2D_FILE_DMA_SET_BUF_TX
#error "[S2D FILE] Missing DMA SET TX BUFFER definition."
#endif

#ifndef S2D_FILE_DMA_INIT_RX
#error "[S2D FILE] Missing DMA INIT RX definition."
#endif

#ifndef S2D_FILE_DMA_START_RX
#error "[S2D FILE] Missing DMA START RX definition."
#endif

#ifndef S2D_FILE_DMA_STOP_RX
#error "[S2D FILE] Missing DMA STOP RX definition."
#endif

#ifndef S2D_FILE_DMA_SET_BUF_RX
#error "[S2D FILE] Missing DMA SET RX BUFFER definition."
#endif

#ifndef S2D_FILE_UART_INIT
#error "[S2D FILE] Missing UART INIT definition."
#endif

#ifndef S2D_FILE_UART_TX_DATA
#error "[S2D FILE] Missing UART TX DATA definition."
#endif

#ifndef S2D_FILE_UART_RX_IN_DEV
#error "[S2D FILE] Missing UART RX INPUT DEVICE definition."
#endif

#ifndef S2D_FILE_UART_DMA
#define S2D_FILE_UART_DMA 0
#endif

#ifndef S2D_FILE_UART_IRQ
#define S2D_FILE_UART_IRQ 1
#endif

#ifndef S2D_FILE_UART_BAUD_RATE
#define S2D_FILE_UART_BAUD_RATE 115200
#endif

#ifndef S2D_FILE_DATA_BUFFER_LENGTH
#define S2D_FILE_DATA_BUFFER_LENGTH 512
#endif

#ifndef S2D_FILE_OUT_TASK_PRIORITY
#define S2D_FILE_OUT_TASK_PRIORITY 10
#endif

#ifndef S2D_FILE_OUT_TASK_WAIT
#define S2D_FILE_OUT_TASK_WAIT S2D_FILE_DATA_BUFFER_LENGTH * ((1/(float)S2D_FILE_UART_BAUD_RATE) * 10)
#endif

#ifndef S2D_FILE_START_FLAG
#define S2D_FILE_START_FLAG 0x80000000
#endif

#ifndef S2D_FILE_ACK_FLAG
#define S2D_FILE_ACK_FLAG 0x40000000
#endif

#ifndef S2D_FILE_RT_FLAG
#define S2D_FILE_RT_FLAG 0x80000000
#endif

#ifndef S2D_FILE_BEGIN_TRANSFER
#define S2D_FILE_BEGIN_TRANSFER '\\'
#endif

#ifndef S2D_FILE_ACK_DATA
#define S2D_FILE_ACK_DATA 'A'
#endif

#ifdef S2D_DISABLE_FILE
#define S2D_FILE_INIT(t)                 {}
#define S2D_FILE_INIT_PACKET(p)          {}
#define S2D_LOG_DMA_NOTIFY_FROM_ISR()    {}
#define S2D_FILE_UART_NOTIFY_FROM_ISR(d) {}
#define S2D_FILE_RESUME_FROM_WORKER()    {}
#else
#define S2D_FILE_INIT(t)                 s2d_init_file(t)
#define S2D_FILE_INIT_PACKET(p)          s2d_init_packet(p)
#define S2D_FILE_DMA_NOTIFY_FROM_ISR()   s2d_file_dma_notify_from_isr()
#define S2D_FILE_UART_NOTIFY_FROM_ISR(d) s2d_file_uart_notify_from_isr(d)
#define S2D_FILE_RESUME_FROM_WORKER()    s2d_file_resume_from_worker()
#endif

enum Packet_Sizes {
    HEADER_LENGTH = 2,
    HEADER_SIZE = HEADER_LENGTH/2 * sizeof(uint32_t) + HEADER_LENGTH/2 * sizeof(float),
    PACKET_SIZE = S2D_FILE_DATA_BUFFER_LENGTH * sizeof(float)
};

typedef struct S2D_Data_Packet {
    uint32_t num_samples;
    float sampling_freq;

    float data[S2D_FILE_DATA_BUFFER_LENGTH];

} s2d_data_packet_s;

enum S2D_File_State {
    WAITING = 0,
    ACTIVE = 1,
    RECEIVING = 2
};

typedef struct S2D_File {
    s2d_data_packet_s* packet;

    // communicate via DMA
    uint8_t header_buffer[HEADER_SIZE];
    uint8_t data_buffer[PACKET_SIZE];

    uint32_t state;

    TaskHandle_t handle;
    TaskHandle_t* worker_handle;
    SemaphoreHandle_t semaphore;

} s2d_file_s;

#endif //APP_S2D_TOOLS_S2D_FILES_H
