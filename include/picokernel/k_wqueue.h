/**
 * 							ULIPE RTOS PICO
 *
 *  @file k_wqueue.h
 *
 *  @brief workqueues job deferrable server  interface
 *
 *
 */

#ifndef __K_WQUEUE_H
#define __K_WQUEUE_H

#if(K_ENABLE_WORKQUEUES > 0)

/**
 *  workqueue deferred job callback
 *
 *  @param work - job data deferred by current workqueue instance
 *  @return none
 *
 */
typedef void (*wqueue_handler_t) (void* work);


/* work data structure */
typedef struct wqueue_job {
	wqueue_handler_t handler;
	k_list_t node;
}wqueue_job_t;

/* workqueue data structure */
typedef struct wqueue {
	tcb_t *thr;
	k_list_t fifo;
}wqueue_t;


/**
 *  @fn WQUEUE_DECLARE()
 *  @brief this macro declares a initialized workqueue object, note the wqueue MUST initialized with
 *  	   wqueue_init in order to become usable even after this macro was called!
 *
 *  @param name - name of the wqueue control structure, is used as parameter to wqueue API
 *  @param priority - priority of the wqueue deferrable server ranged from 0 to 31
 *
 *  @return a wqueue_t control structure ready to use
 */
#define WQUEUE_CONTROL_BLOCK_DECLARE(name, priority)							 \
	THREAD_CONTROL_BLOCK_DECLARE(thread_##name, K_WQUEUES_STACK_SIZE, priority); \
	wqueue_t name = {															 \
		.thr = &thread_##name,													 \
	}																			 \

/**
 *  @fn wqueue_init()
 *  @brief initializes and starts the created workqueue deferrable server
 *
 *  @param wq - workqueue instance user wants to start, previously created
 *
 *  @return k_status_ok or error code in case of invalid value
 */
k_status_t wqueue_init(wqueue_t *wq);



/**
 *  @fn wqueue_submit()
 *  @brief submits a work to be processed by workqueue
 *
 *  @param wq - workqueue to process the job
 *  @param work - work to be handlered by workqueue it can contain custom user data
 *
 *  @return k_status_ok or error code in case of invalid value
 */
k_status_t wqueue_submit(wqueue_t *wq, wqueue_job_t *work);


#endif
#endif /* INCLUDE_PICOKERNEL_WQUEUE_H_ */
