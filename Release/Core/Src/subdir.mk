################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/Font16.c \
../Core/Src/Font32rle.c \
../Core/Src/RFM69_Base.c \
../Core/Src/SSEMesh.c \
../Core/Src/SSEMeshRadio.c \
../Core/Src/boot_memory.c \
../Core/Src/custom_spi.c \
../Core/Src/custom_timer.c \
../Core/Src/custom_uart.c \
../Core/Src/flash_memory.c \
../Core/Src/main.c \
../Core/Src/sse_flash_memory.c \
../Core/Src/sse_meters.c \
../Core/Src/sse_pendencias.c \
../Core/Src/sse_protocol.c \
../Core/Src/sse_util.c \
../Core/Src/st7789.c \
../Core/Src/stm32c0xx_hal_msp.c \
../Core/Src/stm32c0xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32c0xx.c 

OBJS += \
./Core/Src/Font16.o \
./Core/Src/Font32rle.o \
./Core/Src/RFM69_Base.o \
./Core/Src/SSEMesh.o \
./Core/Src/SSEMeshRadio.o \
./Core/Src/boot_memory.o \
./Core/Src/custom_spi.o \
./Core/Src/custom_timer.o \
./Core/Src/custom_uart.o \
./Core/Src/flash_memory.o \
./Core/Src/main.o \
./Core/Src/sse_flash_memory.o \
./Core/Src/sse_meters.o \
./Core/Src/sse_pendencias.o \
./Core/Src/sse_protocol.o \
./Core/Src/sse_util.o \
./Core/Src/st7789.o \
./Core/Src/stm32c0xx_hal_msp.o \
./Core/Src/stm32c0xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32c0xx.o 

C_DEPS += \
./Core/Src/Font16.d \
./Core/Src/Font32rle.d \
./Core/Src/RFM69_Base.d \
./Core/Src/SSEMesh.d \
./Core/Src/SSEMeshRadio.d \
./Core/Src/boot_memory.d \
./Core/Src/custom_spi.d \
./Core/Src/custom_timer.d \
./Core/Src/custom_uart.d \
./Core/Src/flash_memory.d \
./Core/Src/main.d \
./Core/Src/sse_flash_memory.d \
./Core/Src/sse_meters.d \
./Core/Src/sse_pendencias.d \
./Core/Src/sse_protocol.d \
./Core/Src/sse_util.d \
./Core/Src/st7789.d \
./Core/Src/stm32c0xx_hal_msp.d \
./Core/Src/stm32c0xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32c0xx.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o Core/Src/%.su Core/Src/%.cyclo: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0plus -std=gnu11 -DUSE_HAL_DRIVER -DSTM32C011xx -c -I../Core/Inc -I../Drivers/STM32C0xx_HAL_Driver/Inc -I../Drivers/STM32C0xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32C0xx/Include -I../Drivers/CMSIS/Include -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/Font16.cyclo ./Core/Src/Font16.d ./Core/Src/Font16.o ./Core/Src/Font16.su ./Core/Src/Font32rle.cyclo ./Core/Src/Font32rle.d ./Core/Src/Font32rle.o ./Core/Src/Font32rle.su ./Core/Src/RFM69_Base.cyclo ./Core/Src/RFM69_Base.d ./Core/Src/RFM69_Base.o ./Core/Src/RFM69_Base.su ./Core/Src/SSEMesh.cyclo ./Core/Src/SSEMesh.d ./Core/Src/SSEMesh.o ./Core/Src/SSEMesh.su ./Core/Src/SSEMeshRadio.cyclo ./Core/Src/SSEMeshRadio.d ./Core/Src/SSEMeshRadio.o ./Core/Src/SSEMeshRadio.su ./Core/Src/boot_memory.cyclo ./Core/Src/boot_memory.d ./Core/Src/boot_memory.o ./Core/Src/boot_memory.su ./Core/Src/custom_spi.cyclo ./Core/Src/custom_spi.d ./Core/Src/custom_spi.o ./Core/Src/custom_spi.su ./Core/Src/custom_timer.cyclo ./Core/Src/custom_timer.d ./Core/Src/custom_timer.o ./Core/Src/custom_timer.su ./Core/Src/custom_uart.cyclo ./Core/Src/custom_uart.d ./Core/Src/custom_uart.o ./Core/Src/custom_uart.su ./Core/Src/flash_memory.cyclo ./Core/Src/flash_memory.d ./Core/Src/flash_memory.o ./Core/Src/flash_memory.su ./Core/Src/main.cyclo ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/main.su ./Core/Src/sse_flash_memory.cyclo ./Core/Src/sse_flash_memory.d ./Core/Src/sse_flash_memory.o ./Core/Src/sse_flash_memory.su ./Core/Src/sse_meters.cyclo ./Core/Src/sse_meters.d ./Core/Src/sse_meters.o ./Core/Src/sse_meters.su ./Core/Src/sse_pendencias.cyclo ./Core/Src/sse_pendencias.d ./Core/Src/sse_pendencias.o ./Core/Src/sse_pendencias.su ./Core/Src/sse_protocol.cyclo ./Core/Src/sse_protocol.d ./Core/Src/sse_protocol.o ./Core/Src/sse_protocol.su ./Core/Src/sse_util.cyclo ./Core/Src/sse_util.d ./Core/Src/sse_util.o ./Core/Src/sse_util.su ./Core/Src/st7789.cyclo ./Core/Src/st7789.d ./Core/Src/st7789.o ./Core/Src/st7789.su ./Core/Src/stm32c0xx_hal_msp.cyclo ./Core/Src/stm32c0xx_hal_msp.d ./Core/Src/stm32c0xx_hal_msp.o ./Core/Src/stm32c0xx_hal_msp.su ./Core/Src/stm32c0xx_it.cyclo ./Core/Src/stm32c0xx_it.d ./Core/Src/stm32c0xx_it.o ./Core/Src/stm32c0xx_it.su ./Core/Src/syscalls.cyclo ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/syscalls.su ./Core/Src/sysmem.cyclo ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/sysmem.su ./Core/Src/system_stm32c0xx.cyclo ./Core/Src/system_stm32c0xx.d ./Core/Src/system_stm32c0xx.o ./Core/Src/system_stm32c0xx.su

.PHONY: clean-Core-2f-Src

