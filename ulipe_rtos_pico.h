/**
 * 							ULIPE RTOS PICO
 *
 *  @file ulipe_rtos_pico.h
 *
 *  @brief this file is treated as a master file
 *
 *
 */
#ifndef __ULIPE_RTOS_PICO_H
#define __ULIPE_RTOS_PICO_H


/* include all the kernel header files actig as a glue module */
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include "picokernel/inc/k_list.h"
#ifndef __ULIPE_RTOS_KCONFIG_H
	#warning "No config file found, loading default setings"
	#include "ulipe_rtos_def_config.h"
#endif


/* ulipe rtos pico status codes */
typedef enum {
	/* general status */
	k_status_ok = 0,
	k_status_error,
	k_status_invalid_param,
	k_status_sched_locked,
	k_status_illegal_from_isr,

	/* threading status */
	k_thread_rdy,
	k_thread_del,
	k_thread_blk,
	k_thread_susp,

	/* semaphore status */
	k_sema_not_available,
	k_sema_illegal_use_celling,

	/* queue status */
	k_queue_empty,
	k_queue_full,

	/* timer status */
	k_timer_expired,
	k_timer_running,
	k_timer_stopped,
	k_timer_busy,

}k_status_t;


/**
 *  @fn template()
 *  @brief
 *  @param
 *  @return
 */


/* resolves the arch dependend memory size */
#if	(K_ARCH_MEM_WIDTH_BYTE > 0)
typedef uint8_t archtype_t;
#elif(K_ARCH_MEM_WIDTH_HALFWORD > 0)
typedef uint16_t archtype_t;
#elif (K_ARCH_MEM_WIDTH_WORD > 0)
typedef uint32_t archtype_t;
#elif (K_ARCH_MEM_WIDTH_DWORD > 0)
typedef uint64_t archtype_t;
#else
#endif



#include "include/picokernel/k_thread.h"
#include "include/picokernel/k_port.h"
#include "include/picokernel/k_kernel.h"
#include "include/picokernel/k_message.h"
#include "include/picokernel/k_raw_timer.h"
#include "include/picokernel/k_sema.h"



/** assertion mechanism */
static inline void ulipe_assert(bool x)
{
	if(!x){
		port_set_break();
		while(1);
	}
}

#if K_DEBUG > 0
#define ULIPE_ASSERT(x) ulipe_assert(x)
#else
#define ULIPE_ASSERT(x) (void)x
#endif


#endif
