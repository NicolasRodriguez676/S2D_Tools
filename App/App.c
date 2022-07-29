#include "FreeRTOSConfig.h"
#include <FreeRTOS.h>
#include <task.h>

#include <stm32f4xx_ll_usart.h>

#include "clock/systemclock.h"
#include "gpio/led.h"

#include "S2D_Tools/S2D_Log.h"
#include "blinky/blinky.h"

int main(void)
{
    SystemClock_Config();

    NVIC_SetPriorityGrouping(0);

    initLED();

    s2d_init_log();
    S2D_DEBUG("Hello from Main, I sure hope my Tasks are made!");

    xTaskCreate(blinky, "blinky", configMINIMAL_STACK_SIZE * 4, NULL, tskIDLE_PRIORITY + 1, NULL);

    vTaskStartScheduler();

	for (;;);
	
	return 0;
}
