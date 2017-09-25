################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/board.c \
../src/board_sysinit.c 

OBJS += \
./src/board.o \
./src/board_sysinit.o 

C_DEPS += \
./src/board.d \
./src/board_sysinit.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__REDLIB__ -DCORE_M4 -DDEBUG -D__CODE_RED -D__LPC5410X__ -I"/home/venturus/devel/uLipeRtosPico/samples/lpc54102_test_proj/lpc_chip_5410x/inc" -I"/home/venturus/devel/uLipeRtosPico/samples/lpc54102_test_proj/lpc_board_lpcxpresso_54102/inc" -Os -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -fsingle-precision-constant -flto -ffat-lto-objects -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -D__REDLIB__ -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


