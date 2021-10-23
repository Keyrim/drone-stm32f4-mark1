################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Sensors/acc.c \
../Core/Sensors/baro.c \
../Core/Sensors/batterie.c \
../Core/Sensors/gyro.c \
../Core/Sensors/mpu.c \
../Core/Sensors/ms5611.c 

OBJS += \
./Core/Sensors/acc.o \
./Core/Sensors/baro.o \
./Core/Sensors/batterie.o \
./Core/Sensors/gyro.o \
./Core/Sensors/mpu.o \
./Core/Sensors/ms5611.o 

C_DEPS += \
./Core/Sensors/acc.d \
./Core/Sensors/baro.d \
./Core/Sensors/batterie.d \
./Core/Sensors/gyro.d \
./Core/Sensors/mpu.d \
./Core/Sensors/ms5611.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Sensors/%.o: ../Core/Sensors/%.c Core/Sensors/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F405xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

