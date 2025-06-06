################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/calculator.c \
../Core/Src/dma.c \
../Core/Src/drawer.c \
../Core/Src/flash.c \
../Core/Src/games.c \
../Core/Src/gpio.c \
../Core/Src/i2c.c \
../Core/Src/icon.c \
../Core/Src/main.c \
../Core/Src/oled.c \
../Core/Src/oled_driver.c \
../Core/Src/oled_optimize.c \
../Core/Src/oled_ui.c \
../Core/Src/pid.c \
../Core/Src/spi.c \
../Core/Src/stm32f1xx_hal_msp.c \
../Core/Src/stm32f1xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32f1xx.c \
../Core/Src/tim.c \
../Core/Src/videoPlayer.c 

OBJS += \
./Core/Src/calculator.o \
./Core/Src/dma.o \
./Core/Src/drawer.o \
./Core/Src/flash.o \
./Core/Src/games.o \
./Core/Src/gpio.o \
./Core/Src/i2c.o \
./Core/Src/icon.o \
./Core/Src/main.o \
./Core/Src/oled.o \
./Core/Src/oled_driver.o \
./Core/Src/oled_optimize.o \
./Core/Src/oled_ui.o \
./Core/Src/pid.o \
./Core/Src/spi.o \
./Core/Src/stm32f1xx_hal_msp.o \
./Core/Src/stm32f1xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32f1xx.o \
./Core/Src/tim.o \
./Core/Src/videoPlayer.o 

C_DEPS += \
./Core/Src/calculator.d \
./Core/Src/dma.d \
./Core/Src/drawer.d \
./Core/Src/flash.d \
./Core/Src/games.d \
./Core/Src/gpio.d \
./Core/Src/i2c.d \
./Core/Src/icon.d \
./Core/Src/main.d \
./Core/Src/oled.d \
./Core/Src/oled_driver.d \
./Core/Src/oled_optimize.d \
./Core/Src/oled_ui.d \
./Core/Src/pid.d \
./Core/Src/spi.d \
./Core/Src/stm32f1xx_hal_msp.d \
./Core/Src/stm32f1xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32f1xx.d \
./Core/Src/tim.d \
./Core/Src/videoPlayer.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o Core/Src/%.su Core/Src/%.cyclo: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/calculator.cyclo ./Core/Src/calculator.d ./Core/Src/calculator.o ./Core/Src/calculator.su ./Core/Src/dma.cyclo ./Core/Src/dma.d ./Core/Src/dma.o ./Core/Src/dma.su ./Core/Src/drawer.cyclo ./Core/Src/drawer.d ./Core/Src/drawer.o ./Core/Src/drawer.su ./Core/Src/flash.cyclo ./Core/Src/flash.d ./Core/Src/flash.o ./Core/Src/flash.su ./Core/Src/games.cyclo ./Core/Src/games.d ./Core/Src/games.o ./Core/Src/games.su ./Core/Src/gpio.cyclo ./Core/Src/gpio.d ./Core/Src/gpio.o ./Core/Src/gpio.su ./Core/Src/i2c.cyclo ./Core/Src/i2c.d ./Core/Src/i2c.o ./Core/Src/i2c.su ./Core/Src/icon.cyclo ./Core/Src/icon.d ./Core/Src/icon.o ./Core/Src/icon.su ./Core/Src/main.cyclo ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/main.su ./Core/Src/oled.cyclo ./Core/Src/oled.d ./Core/Src/oled.o ./Core/Src/oled.su ./Core/Src/oled_driver.cyclo ./Core/Src/oled_driver.d ./Core/Src/oled_driver.o ./Core/Src/oled_driver.su ./Core/Src/oled_optimize.cyclo ./Core/Src/oled_optimize.d ./Core/Src/oled_optimize.o ./Core/Src/oled_optimize.su ./Core/Src/oled_ui.cyclo ./Core/Src/oled_ui.d ./Core/Src/oled_ui.o ./Core/Src/oled_ui.su ./Core/Src/pid.cyclo ./Core/Src/pid.d ./Core/Src/pid.o ./Core/Src/pid.su ./Core/Src/spi.cyclo ./Core/Src/spi.d ./Core/Src/spi.o ./Core/Src/spi.su ./Core/Src/stm32f1xx_hal_msp.cyclo ./Core/Src/stm32f1xx_hal_msp.d ./Core/Src/stm32f1xx_hal_msp.o ./Core/Src/stm32f1xx_hal_msp.su ./Core/Src/stm32f1xx_it.cyclo ./Core/Src/stm32f1xx_it.d ./Core/Src/stm32f1xx_it.o ./Core/Src/stm32f1xx_it.su ./Core/Src/syscalls.cyclo ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/syscalls.su ./Core/Src/sysmem.cyclo ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/sysmem.su ./Core/Src/system_stm32f1xx.cyclo ./Core/Src/system_stm32f1xx.d ./Core/Src/system_stm32f1xx.o ./Core/Src/system_stm32f1xx.su ./Core/Src/tim.cyclo ./Core/Src/tim.d ./Core/Src/tim.o ./Core/Src/tim.su ./Core/Src/videoPlayer.cyclo ./Core/Src/videoPlayer.d ./Core/Src/videoPlayer.o ./Core/Src/videoPlayer.su

.PHONY: clean-Core-2f-Src

