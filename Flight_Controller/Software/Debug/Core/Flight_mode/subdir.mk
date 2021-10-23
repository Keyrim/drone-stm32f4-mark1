################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Flight_mode/Arming.c \
../Core/Flight_mode/Flight_mode.c \
../Core/Flight_mode/Gyro_Acc_Calibration.c \
../Core/Flight_mode/Manual_Acro.c \
../Core/Flight_mode/Manual_alti_auto.c \
../Core/Flight_mode/Manual_angle.c \
../Core/Flight_mode/On_the_ground.c 

OBJS += \
./Core/Flight_mode/Arming.o \
./Core/Flight_mode/Flight_mode.o \
./Core/Flight_mode/Gyro_Acc_Calibration.o \
./Core/Flight_mode/Manual_Acro.o \
./Core/Flight_mode/Manual_alti_auto.o \
./Core/Flight_mode/Manual_angle.o \
./Core/Flight_mode/On_the_ground.o 

C_DEPS += \
./Core/Flight_mode/Arming.d \
./Core/Flight_mode/Flight_mode.d \
./Core/Flight_mode/Gyro_Acc_Calibration.d \
./Core/Flight_mode/Manual_Acro.d \
./Core/Flight_mode/Manual_alti_auto.d \
./Core/Flight_mode/Manual_angle.d \
./Core/Flight_mode/On_the_ground.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Flight_mode/%.o: ../Core/Flight_mode/%.c Core/Flight_mode/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F405xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

