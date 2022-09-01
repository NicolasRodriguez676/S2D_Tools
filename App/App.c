#include "FreeRTOSConfig.h"
#include <FreeRTOS.h>
#include <task.h>

#include <stm32f4xx_ll_usart.h>
#include <string.h>

#include <arm_math.h>

#include "clock/systemclock.h"
#include "gpio/led.h"

#include "S2D_Tools/S2D_Log.h"
#include "S2D_Tools/S2D_Files.h"

#include "blinky/blinky.h"

extern TaskHandle_t* s2d_file_handle;
TaskHandle_t data_task;

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

    s2d_data_packet_s packet;
    s2d_init_packet(&packet);

    S2D_DEBUG("packet_size=\"%d\"", PACKET_SIZE);
    S2D_DEBUG("header_size=\"%d\"", HEADER_SIZE);

    for (;;)
    {
        xTaskNotifyWait(UINT32_MAX, 0, &notify_value, portMAX_DELAY);

        if (notify_value > 0)
        {
            S2D_DEBUG("Received files out notification");
            S2D_DEBUG("samples=%d", packet.num_samples);

            if (packet.num_samples == 256)
            {
                for (uint32_t i = 0; i < 128; ++i)
                    packet.data[i] = arm_sin_f32(2 * PI * ((float) i / (float) 128));

                for (uint32_t i = 128; i < 256; ++i)
                    packet.data[i] = arm_sin_f32(2 * PI * ((float) i / (float) 128));
            }
            else
            {
                for (uint32_t i = 0; i < 128; ++i)
                    packet.data[i] = arm_sin_f32(2 * PI * ((float) i / (float) 128));
            }

            vTaskResume(*s2d_file_handle);
        }
    }
}