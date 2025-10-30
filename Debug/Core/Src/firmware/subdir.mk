################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Core/Src/firmware/Drv8323rs.cpp 

OBJS += \
./Core/Src/firmware/Drv8323rs.o 

CPP_DEPS += \
./Core/Src/firmware/Drv8323rs.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/firmware/%.o Core/Src/firmware/%.su Core/Src/firmware/%.cyclo: ../Core/Src/firmware/%.cpp Core/Src/firmware/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m0 -std=gnu++14 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F051x8 -c -I../Core/Inc -I../Drivers/STM32F0xx_HAL_Driver/Inc -I../Drivers/STM32F0xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F0xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-firmware

clean-Core-2f-Src-2f-firmware:
	-$(RM) ./Core/Src/firmware/Drv8323rs.cyclo ./Core/Src/firmware/Drv8323rs.d ./Core/Src/firmware/Drv8323rs.o ./Core/Src/firmware/Drv8323rs.su

.PHONY: clean-Core-2f-Src-2f-firmware

