################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/home/venturus/devel/uLipeRtosPico/picokernel/k_kernel.c \
/home/venturus/devel/uLipeRtosPico/picokernel/k_mem_dyn.c \
/home/venturus/devel/uLipeRtosPico/picokernel/k_memp.c \
/home/venturus/devel/uLipeRtosPico/picokernel/k_message.c \
/home/venturus/devel/uLipeRtosPico/picokernel/k_mutex.c \
/home/venturus/devel/uLipeRtosPico/picokernel/k_raw_timer.c \
/home/venturus/devel/uLipeRtosPico/picokernel/k_sema.c \
/home/venturus/devel/uLipeRtosPico/picokernel/k_thread.c \
/home/venturus/devel/uLipeRtosPico/picokernel/k_wqueue.c 

OBJS += \
./uLipeRtosPico/picokernel/k_kernel.o \
./uLipeRtosPico/picokernel/k_mem_dyn.o \
./uLipeRtosPico/picokernel/k_memp.o \
./uLipeRtosPico/picokernel/k_message.o \
./uLipeRtosPico/picokernel/k_mutex.o \
./uLipeRtosPico/picokernel/k_raw_timer.o \
./uLipeRtosPico/picokernel/k_sema.o \
./uLipeRtosPico/picokernel/k_thread.o \
./uLipeRtosPico/picokernel/k_wqueue.o 

C_DEPS += \
./uLipeRtosPico/picokernel/k_kernel.d \
./uLipeRtosPico/picokernel/k_mem_dyn.d \
./uLipeRtosPico/picokernel/k_memp.d \
./uLipeRtosPico/picokernel/k_message.d \
./uLipeRtosPico/picokernel/k_mutex.d \
./uLipeRtosPico/picokernel/k_raw_timer.d \
./uLipeRtosPico/picokernel/k_sema.d \
./uLipeRtosPico/picokernel/k_thread.d \
./uLipeRtosPico/picokernel/k_wqueue.d 


# Each subdirectory must supply rules for building sources it contributes
uLipeRtosPico/picokernel/k_kernel.o: /home/venturus/devel/uLipeRtosPico/picokernel/k_kernel.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DDEBUG -D__CODE_RED -DCORE_M4 -D__USE_LPCOPEN -D__LPC5410X__ -D__REDLIB__ -D__MULTICORE_NONE -DK_USER_CONFIG=1 -I"/home/venturus/devel/uLipeRtosPico/samples/lpc54102_test_proj/lpc_board_lpcxpresso_54102/inc" -I"/home/venturus/devel/uLipeRtosPico/samples/lpc54102_test_proj/lpc_chip_5410x/inc" -I"/home/venturus/devel/uLipeRtosPico/samples/lpc54102_test_proj/work_proj/src" -I/home/venturus/devel/uLipeRtosPico -O0 -fno-common -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -fsingle-precision-constant -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

uLipeRtosPico/picokernel/k_mem_dyn.o: /home/venturus/devel/uLipeRtosPico/picokernel/k_mem_dyn.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DDEBUG -D__CODE_RED -DCORE_M4 -D__USE_LPCOPEN -D__LPC5410X__ -D__REDLIB__ -D__MULTICORE_NONE -DK_USER_CONFIG=1 -I"/home/venturus/devel/uLipeRtosPico/samples/lpc54102_test_proj/lpc_board_lpcxpresso_54102/inc" -I"/home/venturus/devel/uLipeRtosPico/samples/lpc54102_test_proj/lpc_chip_5410x/inc" -I"/home/venturus/devel/uLipeRtosPico/samples/lpc54102_test_proj/work_proj/src" -I/home/venturus/devel/uLipeRtosPico -O0 -fno-common -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -fsingle-precision-constant -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

uLipeRtosPico/picokernel/k_memp.o: /home/venturus/devel/uLipeRtosPico/picokernel/k_memp.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DDEBUG -D__CODE_RED -DCORE_M4 -D__USE_LPCOPEN -D__LPC5410X__ -D__REDLIB__ -D__MULTICORE_NONE -DK_USER_CONFIG=1 -I"/home/venturus/devel/uLipeRtosPico/samples/lpc54102_test_proj/lpc_board_lpcxpresso_54102/inc" -I"/home/venturus/devel/uLipeRtosPico/samples/lpc54102_test_proj/lpc_chip_5410x/inc" -I"/home/venturus/devel/uLipeRtosPico/samples/lpc54102_test_proj/work_proj/src" -I/home/venturus/devel/uLipeRtosPico -O0 -fno-common -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -fsingle-precision-constant -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

uLipeRtosPico/picokernel/k_message.o: /home/venturus/devel/uLipeRtosPico/picokernel/k_message.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DDEBUG -D__CODE_RED -DCORE_M4 -D__USE_LPCOPEN -D__LPC5410X__ -D__REDLIB__ -D__MULTICORE_NONE -DK_USER_CONFIG=1 -I"/home/venturus/devel/uLipeRtosPico/samples/lpc54102_test_proj/lpc_board_lpcxpresso_54102/inc" -I"/home/venturus/devel/uLipeRtosPico/samples/lpc54102_test_proj/lpc_chip_5410x/inc" -I"/home/venturus/devel/uLipeRtosPico/samples/lpc54102_test_proj/work_proj/src" -I/home/venturus/devel/uLipeRtosPico -O0 -fno-common -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -fsingle-precision-constant -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

uLipeRtosPico/picokernel/k_mutex.o: /home/venturus/devel/uLipeRtosPico/picokernel/k_mutex.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DDEBUG -D__CODE_RED -DCORE_M4 -D__USE_LPCOPEN -D__LPC5410X__ -D__REDLIB__ -D__MULTICORE_NONE -DK_USER_CONFIG=1 -I"/home/venturus/devel/uLipeRtosPico/samples/lpc54102_test_proj/lpc_board_lpcxpresso_54102/inc" -I"/home/venturus/devel/uLipeRtosPico/samples/lpc54102_test_proj/lpc_chip_5410x/inc" -I"/home/venturus/devel/uLipeRtosPico/samples/lpc54102_test_proj/work_proj/src" -I/home/venturus/devel/uLipeRtosPico -O0 -fno-common -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -fsingle-precision-constant -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

uLipeRtosPico/picokernel/k_raw_timer.o: /home/venturus/devel/uLipeRtosPico/picokernel/k_raw_timer.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DDEBUG -D__CODE_RED -DCORE_M4 -D__USE_LPCOPEN -D__LPC5410X__ -D__REDLIB__ -D__MULTICORE_NONE -DK_USER_CONFIG=1 -I"/home/venturus/devel/uLipeRtosPico/samples/lpc54102_test_proj/lpc_board_lpcxpresso_54102/inc" -I"/home/venturus/devel/uLipeRtosPico/samples/lpc54102_test_proj/lpc_chip_5410x/inc" -I"/home/venturus/devel/uLipeRtosPico/samples/lpc54102_test_proj/work_proj/src" -I/home/venturus/devel/uLipeRtosPico -O0 -fno-common -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -fsingle-precision-constant -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

uLipeRtosPico/picokernel/k_sema.o: /home/venturus/devel/uLipeRtosPico/picokernel/k_sema.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DDEBUG -D__CODE_RED -DCORE_M4 -D__USE_LPCOPEN -D__LPC5410X__ -D__REDLIB__ -D__MULTICORE_NONE -DK_USER_CONFIG=1 -I"/home/venturus/devel/uLipeRtosPico/samples/lpc54102_test_proj/lpc_board_lpcxpresso_54102/inc" -I"/home/venturus/devel/uLipeRtosPico/samples/lpc54102_test_proj/lpc_chip_5410x/inc" -I"/home/venturus/devel/uLipeRtosPico/samples/lpc54102_test_proj/work_proj/src" -I/home/venturus/devel/uLipeRtosPico -O0 -fno-common -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -fsingle-precision-constant -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

uLipeRtosPico/picokernel/k_thread.o: /home/venturus/devel/uLipeRtosPico/picokernel/k_thread.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DDEBUG -D__CODE_RED -DCORE_M4 -D__USE_LPCOPEN -D__LPC5410X__ -D__REDLIB__ -D__MULTICORE_NONE -DK_USER_CONFIG=1 -I"/home/venturus/devel/uLipeRtosPico/samples/lpc54102_test_proj/lpc_board_lpcxpresso_54102/inc" -I"/home/venturus/devel/uLipeRtosPico/samples/lpc54102_test_proj/lpc_chip_5410x/inc" -I"/home/venturus/devel/uLipeRtosPico/samples/lpc54102_test_proj/work_proj/src" -I/home/venturus/devel/uLipeRtosPico -O0 -fno-common -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -fsingle-precision-constant -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

uLipeRtosPico/picokernel/k_wqueue.o: /home/venturus/devel/uLipeRtosPico/picokernel/k_wqueue.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DDEBUG -D__CODE_RED -DCORE_M4 -D__USE_LPCOPEN -D__LPC5410X__ -D__REDLIB__ -D__MULTICORE_NONE -DK_USER_CONFIG=1 -I"/home/venturus/devel/uLipeRtosPico/samples/lpc54102_test_proj/lpc_board_lpcxpresso_54102/inc" -I"/home/venturus/devel/uLipeRtosPico/samples/lpc54102_test_proj/lpc_chip_5410x/inc" -I"/home/venturus/devel/uLipeRtosPico/samples/lpc54102_test_proj/work_proj/src" -I/home/venturus/devel/uLipeRtosPico -O0 -fno-common -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -fsingle-precision-constant -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


