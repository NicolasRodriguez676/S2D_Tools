#include "FreeRTOSConfig.h"
#include <FreeRTOS.h>
#include <task.h>

#include <stm32f4xx_ll_usart.h>

#include "clock/systemclock.h"
#include "gpio/led.h"

#include "S2D_Tools/S2D_Log.h"
#include "S2D_Tools/S2D_Files.h"

#include "blinky/blinky.h"

TaskHandle_t data_task;
extern TaskHandle_t* s2d_file_handle;
void get_data();

int main(void)
{
    SystemClock_Config();

    NVIC_SetPriorityGrouping(0);

    initLED();

    S2D_LOG_INIT();
    S2D_SET_LEVEL_INFO();

    xTaskCreate(blinky, "blinky", configMINIMAL_STACK_SIZE * 4, NULL, tskIDLE_PRIORITY + 1, NULL);
    xTaskCreate(get_data, "get_data", configMINIMAL_STACK_SIZE * 8, NULL, tskIDLE_PRIORITY + 1, &data_task);

    s2d_init_file(&data_task);

    vTaskStartScheduler();

	for (;;);
	
	return 0;
}

void get_data()
{
    uint32_t notify_value;

    for (;;)
    {
        xTaskNotifyWait(UINT32_MAX, 0, &notify_value, portMAX_DELAY);

        if (notify_value > 0)
        {
            S2D_DEBUG("Received files out notification");

            s2d_data_packet_s packet;

            packet.header1[0] = 1.0f;

            vTaskDelay(pdMS_TO_TICKS(2500));
            vTaskResume(*s2d_file_handle);
        }
    }
}