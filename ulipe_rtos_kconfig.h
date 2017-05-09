/**
 * 							ULIPE RTOS PICO
 *
 *  @file ulipe_rtos_kconfig.h
 *
 *  @brief kernel configuration file
 *
 *
 */
#ifndef __ULIPE_RTOS_KCONFIG_H
#define __ULIPE_RTOS_KCONFIG_H

/* kernel debugging */
#define K_DEBUG							1

/* architecture definition */
#define ARCH_TYPE_ARM_CM0				0
#define ARCH_TYPE_ARM_CM3_4_7			1
#define ARCH_TYPE_ARM_CM4_7_F			0


/* architecture data width */
#define K_ARCH_MEM_WIDTH_BYTE			0
#define K_ARCH_MEM_WIDTH_HALFWORD		0
#define K_ARCH_MEM_WIDTH_WORD			1
#define K_ARCH_MEM_WIDTH_DWORD			0

/* general kernel configuration */
#define K_MINIMAL_STACK_VAL				16
#define K_TIMER_DISPATCHER_PRIORITY		4
#define K_TIMER_DISPATCHER_STACK_SIZE	128
#define K_ENABLE_SEMAPHORE				1
#define K_ENABLE_MESSAGING				0
#define K_ENABLE_TIMERS					0


/* timer related */
#define K_MACHINE_CLOCK					48000000
#define K_TICKER_FREQUENCY				1000


#endif
