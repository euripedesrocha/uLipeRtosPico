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
	bool created;
	k_work_list_t threads_pending;
}ksema_t;


/**
 *  @fn SEMAPHORE_BLOCK_DECLARE()
 *  @brief declare a fully initialized semaphore control block
 *
 *  @param name - name of sempahore block control structure, used as parameters on Semaphore API
 *  @param initial - initial counting available of semaphore
 *  @param limit_val - counting of maximum available acquisitions of semaphore
 *
 *  @return a ksema_t control structure ready to use
 */
#define SEMAPHORE_BLOCK_DECLARE(name,initial,limit_val)			\
	static ksema_t name = {										\
			.cnt=initial,										\
			.limit=limit_val,									\
			.created=false,										\
	}


/**
 *  @fn semaphore_take()
 *  @brief take a semaphore, if not available blocks the current task
 *
 *  @param s - semaphore to be acquired
 *
 *  @return k_status_ok or error code in case of invalid value
 */
k_status_t semaphore_take(ksema_t *s);


/**
 *  @fn semaphore_give()
 *  @brief release a previously acquired semaphore
 *
 *  @param s- semaphore acquired
 *  @param count - amount of acquisitions to be released (cannot be greater than limit value)
 *
 *  @return k_status_ok or error code in case of invalid value
 */
k_status_t semaphore_give(ksema_t *s, uint32_t count);






#endif
#endif
