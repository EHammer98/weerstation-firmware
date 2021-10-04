################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/c-circus/Src/circularbuffer.c \
../Drivers/c-circus/Src/circularuart.c 

OBJS += \
./Drivers/c-circus/Src/circularbuffer.o \
./Drivers/c-circus/Src/circularuart.o 

C_DEPS += \
./Drivers/c-circus/Src/circularbuffer.d \
./Drivers/c-circus/Src/circularuart.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/c-circus/Src/%.o: ../Drivers/c-circus/Src/%.c Drivers/c-circus/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F030x8 -c -I../Core/Inc -I../Drivers/STM32F0xx_HAL_Driver/Inc -I../Drivers/STM32F0xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F0xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM0 -I../Drivers/esp8266-iot-driver/Inc -I../Drivers/c-circus/Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

