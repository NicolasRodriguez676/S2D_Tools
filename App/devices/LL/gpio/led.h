#ifndef _LED_H_
#define _LED_H_

#include "stm32f4xx_ll_bus.h"
#include "stm32f4xx_ll_gpio.h"

// STM32F4-Nucleo green led - PA5
#define LED_PORT                GPIOA
#define LED_PIN                 LL_GPIO_PIN_5
#define LED_PORT_CLK_ENABLE()   do{ RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN; } while(0)

void initLED();

#endif
