################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/adc_5410x.c \
../src/chip_5410x.c \
../src/clock_5410x.c \
../src/crc_5410x.c \
../src/dma_5410x.c \
../src/fifo_5410x.c \
../src/fpu_init.c \
../src/gpio_5410x.c \
../src/gpiogroup_5410x.c \
../src/i2cm_5410x.c \
../src/i2cs_5410x.c \
../src/iap.c \
../src/iocon_5410x.c \
../src/pinint_5410x.c \
../src/pll_5410x.c \
../src/ring_buffer.c \
../src/ritimer_5410x.c \
../src/rtc_ut.c \
../src/sct_5410x.c \
../src/sct_pwm_5410x.c \
../src/spi_common_5410x.c \
../src/spim_5410x.c \
../src/spis_5410x.c \
../src/stopwatch_5410x.c \
../src/syscon_5410x.c \
../src/sysinit_5410x.c \
../src/timer_5410x.c \
../src/uart_5410x.c \
../src/wwdt_5410x.c 

OBJS += \
./src/adc_5410x.o \
./src/chip_5410x.o \
./src/clock_5410x.o \
./src/crc_5410x.o \
./src/dma_5410x.o \
./src/fifo_5410x.o \
./src/fpu_init.o \
./src/gpio_5410x.o \
./src/gpiogroup_5410x.o \
./src/i2cm_5410x.o \
./src/i2cs_5410x.o \
./src/iap.o \
./src/iocon_5410x.o \
./src/pinint_5410x.o \
./src/pll_5410x.o \
./src/ring_buffer.o \
./src/ritimer_5410x.o \
./src/rtc_ut.o \
./src/sct_5410x.o \
./src/sct_pwm_5410x.o \
./src/spi_common_5410x.o \
./src/spim_5410x.o \
./src/spis_5410x.o \
./src/stopwatch_5410x.o \
./src/syscon_5410x.o \
./src/sysinit_5410x.o \
./src/timer_5410x.o \
./src/uart_5410x.o \
./src/wwdt_5410x.o 

C_DEPS += \
./src/adc_5410x.d \
./src/chip_5410x.d \
./src/clock_5410x.d \
./src/crc_5410x.d \
./src/dma_5410x.d \
./src/fifo_5410x.d \
./src/fpu_init.d \
./src/gpio_5410x.d \
./src/gpiogroup_5410x.d \
./src/i2cm_5410x.d \
./src/i2cs_5410x.d \
./src/iap.d \
./src/iocon_5410x.d \
./src/pinint_5410x.d \
./src/pll_5410x.d \
./src/ring_buffer.d \
./src/ritimer_5410x.d \
./src/rtc_ut.d \
./src/sct_5410x.d \
./src/sct_pwm_5410x.d \
./src/spi_common_5410x.d \
./src/spim_5410x.d \
./src/spis_5410x.d \
./src/stopwatch_5410x.d \
./src/syscon_5410x.d \
./src/sysinit_5410x.d \
./src/timer_5410x.d \
./src/uart_5410x.d \
./src/wwdt_5410x.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__REDLIB__ -DCORE_M4 -DDEBUG -D__CODE_RED -D__LPC5410X__ -I"/home/venturus/devel/uLipeRtosPico/samples/lpc54102_test_proj/lpc_chip_5410x/inc" -Os -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -fsingle-precision-constant -flto -ffat-lto-objects -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -D__REDLIB__ -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


