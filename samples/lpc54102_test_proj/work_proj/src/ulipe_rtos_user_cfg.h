#ifndef __ULIPE_RTOS_USER_CFG_H
#define __ULIPE_RTOS_USER_CFG_H

/* kernel debugging */
#define K_DEBUG 						1
/* architecture definition */
#define ARCH_TYPE_ARM_CM0				0
#define ARCH_TYPE_AVR_TINY				0
#define ARCH_TYPE_ARM_CM3_4_7			1

#define K_SVC_MAX_PRIO					255
#define K_MACHINE_CLOCK					12000000
#define K_TICKER_RATE					1000

/* architecture data width */
#define K_ARCH_MEM_WIDTH_WORD	        1

/* general kernel configuration */
#define K_MINIMAL_STACK_VAL	            64
#define K_TIMER_DISPATCHER_PRIORITY	    24
#define K_TIMER_DISPATCHER_STACK_SIZE	128
#define K_ENABLE_SEMAPHORE				1
#define K_ENABLE_MESSAGING				1
#define K_ENABLE_TICKER					1
#define K_ENABLE_TIMERS					1
#define K_ENABLE_TIMER_GENERIC_SUPPORT	1
#define K_ENABLE_MEMORY_POOLS			1
#define K_ENABLE_MUTEX					1
#define K_ENABLE_WORKQUEUES				1
#define K_WQUEUES_STACK_SIZE 			128

/* Tickless idle support */
#define K_ENABLE_TICKLESS_IDLE			0
#define K_MAX_LOW_POWER_PERIOD			(500)


#endif
