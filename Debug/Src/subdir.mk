################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/015uart_tx.c 

OBJS += \
./Src/015uart_tx.o 

C_DEPS += \
./Src/015uart_tx.d 


# Each subdirectory must supply rules for building sources it contributes
Src/015uart_tx.o: ../Src/015uart_tx.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DSTM32 -DSTM32F4 -DSTM32F446RETx -DDEBUG -DNUCLEO_F446RE -c -I"C:/Users/Nvidia/Documents/MCU1 Course/MCU1/stm32f4xx_drivers/drivers/Inc" -I../Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Src/015uart_tx.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

