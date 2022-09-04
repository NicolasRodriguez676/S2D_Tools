#include <stm32f4xx_ll_bus.h>
#include <stm32f4xx_ll_tim.h>

void init_timer2()
{
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM2);

    LL_TIM_InitTypeDef TIM_InitStruct = { 0 };

    TIM_InitStruct.Prescaler = 17;
    TIM_InitStruct.CounterMode = LL_TIM_COUNTERMODE_UP;
    TIM_InitStruct.Autoreload = UINT32_MAX;
    TIM_InitStruct.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
    LL_TIM_Init(TIM2, &TIM_InitStruct);

    LL_TIM_EnableARRPreload(TIM2);
    LL_TIM_SetClockSource(TIM2, LL_TIM_CLOCKSOURCE_INTERNAL);

    LL_TIM_OC_InitTypeDef TIM_OC_InitStruct = { 0 };

    TIM_OC_InitStruct.OCMode = LL_TIM_OCMODE_FROZEN;
    TIM_OC_InitStruct.OCState = LL_TIM_OCSTATE_DISABLE;
    TIM_OC_InitStruct.OCNState = LL_TIM_OCSTATE_DISABLE;
    TIM_OC_InitStruct.CompareValue = 0;
    TIM_OC_InitStruct.OCPolarity = LL_TIM_OCPOLARITY_HIGH;
    LL_TIM_OC_Init(TIM2, LL_TIM_CHANNEL_CH4, &TIM_OC_InitStruct);

    LL_TIM_DisableMasterSlaveMode(TIM2);
    LL_TIM_SetTriggerOutput(TIM2, LL_TIM_TRGO_ENABLE);

}

uint32_t get_counter_value()
{
    return LL_TIM_GetCounter(TIM2);
}

void enable_counter()
{
    LL_TIM_EnableCounter(TIM2);
}

void disable_counter()
{
    LL_TIM_DisableCounter(TIM2);
}

void set_counter_zero()
{
    LL_TIM_WriteReg(TIM2, CNT, 0);
}


