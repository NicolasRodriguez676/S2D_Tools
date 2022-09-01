#ifndef APP_S2D_TOOLS_S2D_FILES_H
#define APP_S2D_TOOLS_S2D_FILES_H

#include <FreeRTOS.h>
#include <semphr.h>

#include "S2D_Tools_Config.h"
#include "S2D_File_Transfer/Files_Out_Task/Files_Out.h"
#include "S2D_File_Transfer/Files/Files.h"

#ifndef S2D_FILE_UART_BAUD_RATE
#define S2D_FILE_UART_BAUD_RATE  115200
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
    TaskHandle_t* rt_handle;
    SemaphoreHandle_t semaphore;

} s2d_file_s;

#endif //APP_S2D_TOOLS_S2D_FILES_H
