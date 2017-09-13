/**
 * 							ULIPE RTOS PICO
 *
 *  @file k_mutex.h
 *
 *  @brief mutual exclusion semaphore stuff
 *
 */
#ifndef __K_MUTEX_H
#define __K_MUTEX_H

#if K_ENABLE_MUTEX > 0


/** define mutex celing priority value */
#define K_MUTEX_PRIO_CEIL_VAL	(K_PRIORITY_LEVELS - 8)



/* semaphore control block structure */
typedef struct kmutex{
	bool created;
	uint8_t owner_prio;
	tcb_t *thr_owner;
	k_work_list_t threads_pending;
}kmutex_t;


/**
 *  @fn MUTEX_BLOCK_DECLARE()
 *  @brief declare a fully initialized mutex control block
 *  @param
 *  @return
 */
#define MUTEX_BLOCK_DECLARE(name)							\
	static kmutex_t name = {								\
			.thr_owner=NULL,								\
			.owner_prio=0,									\
			.created=false,									\
	}

/**
 *  @fn mutex_take()
 *  @brief take a mutex, block if not currently available or returns if try is set to true
 *  @param
 *  @return
 */
k_status_t mutex_take(kmutex_t *m, bool try);


/**
 *  @fn mutex_give()
 *  @brief release a mutex
 *  @param
 *  @return
 */
k_status_t mutex_give(kmutex_t *m);



#endif
#endif
