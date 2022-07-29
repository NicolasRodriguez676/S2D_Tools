cmake_minimum_required(VERSION 3.22)

if (NOT DEFINED ENV{CMAKE_STM32_SCRIPTS})
    message(FATAL_ERROR "Could not find STM32 CMake scripts path")
endif()

set(CMAKE_MODULE_PATH  $ENV{CMAKE_STM32_SCRIPTS})
set(CMAKE_TOOLCHAIN_FILE $ENV{CMAKE_STM32_SCRIPTS}/stm32_gcc.cmake)

project(APP_S2D_TOOLS C ASM)
set(MY_APP_NAME APP)

find_package(CMSIS COMPONENTS STM32F446RE STM32F4_M4 RTOS REQUIRED)
find_package(HAL COMPONENTS STM32F4 LL_DMA LL_RCC LL_GPIO LL_USART LL_SPI LL_ADC LL_UTILS LL_TIM REQUIRED)
find_package(FreeRTOS COMPONENTS ARM_CM4F STM32F4 REQUIRED)

add_compile_options(
        -mcpu=cortex-m4
        -mfpu=fpv4-sp-d16
        -mfloat-abi=hard
        -fno-builtin-printf
        -DUSE_FULL_LL_DRIVER
        -Os
        -std=c11
        -Wall
        -Wextra
        -Wno-unused-parameter
        -Wpedantic
        )

add_executable(${MY_APP_NAME})

add_subdirectory(printf)
add_subdirectory(App)
add_subdirectory(S2D_Tools/S2D_Logger)

set(CMAKE_INCLUDE_CURRENT_DIR TRUE)

target_include_directories(${MY_APP_NAME}
        PUBLIC
            S2D_Tools

            App/config
            App/lib
            App/tasks
            App/devices/LL
        )

target_link_libraries(${MY_APP_NAME}
        PUBLIC
        CMSIS::STM32::F446RE
        CMSIS::STM32::F4::RTOS

        FreeRTOS::STM32::F4::ARM_CM4F
        FreeRTOS::STM32::F4::Timers
        FreeRTOS::STM32::F4::Heap::3

        HAL::STM32::F4::LL_RCC
        HAL::STM32::F4::LL_GPIO
        HAL::STM32::F4::LL_USART
        HAL::STM32::F4::LL_DMA
        HAL::STM32::F4::LL_UTILS
        HAL::STM32::F4::LL_TIM

        STM32::NoSys

        PRINTF
        )

stm32_print_size_of_target(${MY_APP_NAME})
stm32_generate_binary_file(${MY_APP_NAME})