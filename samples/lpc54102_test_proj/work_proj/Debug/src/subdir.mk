################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/boot_multicore_slave.c \
../src/cr_startup_lpc5410x.c \
../src/crp.c \
../src/sysinit.c \
../src/work_proj.c 

OBJS += \
./src/boot_multicore_slave.o \
./src/cr_startup_lpc5410x.o \
./src/crp.o \
./src/sysinit.o \
./src/work_proj.o 

C_DEPS += \
./src/boot_multicore_slave.d \
./src/cr_startup_lpc5410x.d \
./src/crp.d \
./src/sysinit.d \
./src/work_proj.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DDEBUG -D__CODE_RED -DCORE_M4 -D__USE_LPCOPEN -D__LPC5410X__ -D__REDLIB__ -D__MULTICORE_NONE -DK_USER_CONFIG=1 -I"/home/venturus/devel/uLipeRtosPico/samples/lpc54102_test_proj/lpc_board_lpcxpresso_54102/inc" -I"/home/venturus/devel/uLipeRtosPico/samples/lpc54102_test_proj/lpc_chip_5410x/inc" -I"/home/venturus/devel/uLipeRtosPico/samples/lpc54102_test_proj/work_proj/src" -I/home/venturus/devel/uLipeRtosPico -O0 -fno-common -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -fsingle-precision-constant -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


