################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Motor_Mixer/Motor_Mixer.c 

OBJS += \
./Core/Motor_Mixer/Motor_Mixer.o 

C_DEPS += \
./Core/Motor_Mixer/Motor_Mixer.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Motor_Mixer/%.o: ../Core/Motor_Mixer/%.c Core/Motor_Mixer/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DARM_MATH_CM4 -DUSE_HAL_DRIVER -DSTM32F405xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Motor_Mixer

clean-Core-2f-Motor_Mixer:
	-$(RM) ./Core/Motor_Mixer/Motor_Mixer.d ./Core/Motor_Mixer/Motor_Mixer.o

.PHONY: clean-Core-2f-Motor_Mixer

