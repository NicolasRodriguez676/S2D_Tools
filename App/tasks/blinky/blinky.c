#include <FreeRTOS.h>
#include <task.h>

#include "gpio/led.h"
#include "S2D_Log.h"

void blinky()
{
    S2D_DEBUG("Initialize Blinky Task!");

    for(;;)
	{
        S2D_DEBUG("Printing a really long message to trigger a skipped log message. And this one still needs to be even longer! I made it!");

        vTaskDelay(1500);
        LL_GPIO_TogglePin(LED_PORT, LED_PIN);

	}
}
