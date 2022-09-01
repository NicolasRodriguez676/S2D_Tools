#ifndef APP_S2D_TOOLS_UART3_H
#define APP_S2D_TOOLS_UART3_H

#include <FreeRTOS.h>
#include <task.h>

void init_uart3(uint32_t baud_rate);
void set_uart_input_dev(uint32_t dev);
void get_file_handle(TaskHandle_t* handle);

#endif //APP_S2D_TOOLS_UART3_H
