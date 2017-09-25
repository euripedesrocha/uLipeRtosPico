################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/home/venturus/devel/uLipeRtosPico/arch/avr_tiny/k_port_avr_tiny.c 

S_UPPER_SRCS += \
/home/venturus/devel/uLipeRtosPico/arch/avr_tiny/k_port_avr_tiny_a.S 

OBJS += \
./uLipeRtosPico/arch/avr_tiny/k_port_avr_tiny.o \
./uLipeRtosPico/arch/avr_tiny/k_port_avr_tiny_a.o 

C_DEPS += \
./uLipeRtosPico/arch/avr_tiny/k_port_avr_tiny.d 


# Each subdirectory must supply rules for building sources it contributes
uLipeRtosPico/arch/avr_tiny/k_port_avr_tiny.o: /home/venturus/devel/uLipeRtosPico/arch/avr_tiny/k_port_avr_tiny.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DDEBUG -D__CODE_RED -DCORE_M4 -D__USE_LPCOPEN -D__LPC5410X__ -D__REDLIB__ -D__MULTICORE_NONE -DK_USER_CONFIG=1 -I"/home/venturus/devel/uLipeRtosPico/samples/lpc54102_test_proj/lpc_board_lpcxpresso_54102/inc" -I"/home/venturus/devel/uLipeRtosPico/samples/lpc54102_test_proj/lpc_chip_5410x/inc" -I"/home/venturus/devel/uLipeRtosPico/samples/lpc54102_test_proj/work_proj/src" -I/home/venturus/devel/uLipeRtosPico -O0 -fno-common -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -fsingle-precision-constant -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

uLipeRtosPico/arch/avr_tiny/k_port_avr_tiny_a.o: /home/venturus/devel/uLipeRtosPico/arch/avr_tiny/k_port_avr_tiny_a.S
	@echo 'Building file: $<'
	@echo 'Invoking: MCU Assembler'
	arm-none-eabi-gcc -c -x assembler-with-cpp -DDEBUG -D__CODE_RED -DCORE_M4 -D__USE_LPCOPEN -D__LPC5410X__ -D__REDLIB__ -D__MULTICORE_NONE -I"/home/venturus/devel/uLipeRtosPico/samples/lpc54102_test_proj/lpc_board_lpcxpresso_54102/inc" -I"/home/venturus/devel/uLipeRtosPico/samples/lpc54102_test_proj/lpc_chip_5410x/inc" -g3 -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -specs=redlib.specs -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


