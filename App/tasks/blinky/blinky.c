#include <FreeRTOS.h>
#include <task.h>

#include "gpio/led.h"
#include "S2D_Log.h"

void blinky()
{
    S2D_INFO("Initialize Blinky Task!");

    for(;;)
	{
        vTaskDelay(pdMS_TO_TICKS(1500));

        S2D_TIME_START();
        LL_GPIO_TogglePin(LED_PORT, LED_PIN);
        S2D_TIME_STOP();
	}
}
