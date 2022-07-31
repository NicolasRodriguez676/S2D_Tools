#include "FreeRTOSConfig.h"
#include <FreeRTOS.h>
#include <task.h>

#include <stm32f4xx_ll_usart.h>

#include "clock/systemclock.h"
#include "gpio/led.h"

#include "S2D_Tools/S2D_Log.h"
#include "blinky/blinky.h"

int main()
{
    SystemClock_Config();

    NVIC_SetPriorityGrouping(0);

    initLED();

    S2D_LOG_INIT();
    S2D_LOG_LEVEL_SET(S2D_LOG_INFO);

    xTaskCreate(blinky, "blinky", configMINIMAL_STACK_SIZE * 4, NULL, tskIDLE_PRIORITY + 1, NULL);

    vTaskStartScheduler();

	for (;;);
	
	return 0;
}
