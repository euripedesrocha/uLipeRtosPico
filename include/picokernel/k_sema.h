/**
 * 							ULIPE RTOS PICO
 *
 *  @file k_sema.h
 *
 *  @brief basic semaphore usage header file
 *
 */
#ifndef __K_SEMA_H
#define __K_SEMA_H

#if K_ENABLE_SEMAPHORE > 0

/* semaphore control block structure */
typedef struct ksema{
	archtype_t cnt;
	archtype_t limit;
	bool prio_ceilling;
	bool created;
	uint8_t prio;
	k_work_list_t threads_pending;
}ksema_t;


/**
 *  @fn SEMAPHORE_BLOCK_DECLARE()
 *  @brief declare a fully initialized semaphore control block
 *  @param
 *  @return
 */
#define SEMAPHORE_BLOCK_DECLARE(name,initial,limit_val)			\
	static ksema_t name = {										\
			.cnt=initial,										\
			.limit=limit_val,									\
			.prio=K_SYS_THREAD_PRIO,							\
			.prio_ceilling=false,								\
			.created=false,										\
	}


/**
 *  @fn semaphore_take()
 *  @brief take a semaphore, stops execution until available
 *  @param
 *  @return
 */
k_status_t semaphore_take(ksema_t *s);


/**
 *  @fn semaphore_take_and_ceil()
 *  @brief
 *  @param
 *  @return
 */
k_status_t semaphore_take_and_ceil(ksema_t *s);

/**
 *  @fn semaphore_give()
 *  @brief release a semaphore incrementing its count
 *  @param
 *  @return
 */
k_status_t semaphore_give(ksema_t *s, uint32_t count);




#endif
#endif
