#ifndef APP_S2D_TOOLS_S2D_FILES_H
#define APP_S2D_TOOLS_S2D_FILES_H

#include <FreeRTOS.h>
#include <semphr.h>

#include "S2D_Tools_Config.h"
#include "S2D_File_Transfer/Files_Out_Task/Files_Out.h"
#include "S2D_File_Transfer/Files/Files.h"

#ifndef S2D_FILE_OUT_TASK_PRIORITY
#define S2D_FILE_OUT_TASK_PRIORITY 10
#endif

#ifndef S2D_FILE_OUT_TASK_WAIT
#define S2D_FILE_OUT_TASK_WAIT 1000
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

#ifndef S2D_FILE_ACK_DATA
#define S2D_FILE_ACK_DATA 'A'
#endif

#ifndef S2D_FILE_UART_BAUD_RATE
#define S2D_FILE_UART_BAUD_RATE  115200
#endif

#ifndef S2D_FILE_DATA_BUFFER_SIZE
#define S2D_FILE_DATA_BUFFER_SIZE 64
#endif

enum Packet_Sizes {
    HEADER_LENGTH1 = 1,
    HEADER_LENGTH2 = 1,
    CRC32_LENGTH = 1,
    DATA_LENGTH = S2D_FILE_DATA_BUFFER_SIZE,
    PACKET_SIZE = (HEADER_LENGTH1 + HEADER_LENGTH2) * sizeof(float) + CRC32_LENGTH * sizeof(uint32_t) + DATA_LENGTH
};

typedef struct S2D_Data_Packet {
    float header1[HEADER_LENGTH1];
    uint32_t header2[HEADER_LENGTH2];
    float data[DATA_LENGTH];
    uint32_t crc32;

} s2d_data_packet_s;

typedef struct S2D_File {
    s2d_data_packet_s packet;

    uint8_t buffer[PACKET_SIZE];
    uint32_t length;

    TaskHandle_t handle;
    TaskHandle_t* rt_handle;
    SemaphoreHandle_t semaphore;

} s2d_file_s;

#endif //APP_S2D_TOOLS_S2D_FILES_H
