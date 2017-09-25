/**
 * 							ULIPE RTOS PICO
 *
 *  @file k_thread.h
 *
 *  @brief threading managament core header file
 *
 *
 */

#ifndef __K_THREAD_H
#define __K_THREAD_H


/* define the stask status, not used in user application*/
#define K_THR_SUSPENDED  			(0x01)
#define K_THR_PEND_SEMA  			(0x02)
#define K_THR_PEND_MSG	 			(0x04)
#define K_THR_PEND_TMR	 			(0x08)
#define K_THR_PEND_SIGNAL_ALL 		(0x10)
#define K_THR_PEND_SIGNAL_ANY 		(0x20)
#define K_THR_PEND_SIGNAL_ALL_C 	(0x40)
#define K_THR_PEND_SIGNAL_ANY_C 	(0x80)
#define K_THR_PEND_TICKER	 		(0x100)
#define K_THR_PEND_MTX		 		(0x200)



/* define signals options */
typedef enum {
	k_wait_match_pattern = 0,
	k_wait_match_any,
	k_match_pattern_consume,
	k_match_any_consume,
}thread_signal_opt_t;

/* thread function definition
 *
 * @param arg - custom data provided by application during thread creation
 *
 * @return threads should never return
 *
 */
typedef void (*thread_t) (void *arg);


/* thread control block data structure */
typedef struct ktcb{
	archtype_t *stack_top;
	archtype_t *stack_base;
	archtype_t stk_usage;
	uint16_t thread_wait;
	uint8_t thread_prio;
	bool created;
	uint32_t stack_size;
	uint32_t wake_tick;
	archtype_t signals_wait;
	archtype_t signals_actual;
	archtype_t timer_wait;
	k_list_t thr_link;
}tcb_t;


/**
 *  @fn THREAD_CONTROL_BLOCK_DECLARE()
 *  @brief declares a full initialized thread control block ready to be created
 *
 *  @param name - name of thread control structure created, used as parameter to threads API
 *  @param stack_size_val - size of stack in archtype_t entries (not in bytes!)
 *  @param priority - priority of the thread after created range from 0 to 31
 *
 *  @return a tcb_t control structure ready to use
 */

#define THREAD_CONTROL_BLOCK_DECLARE(name, stack_size_val, priority) 			\
		static archtype_t stack_##name[stack_size_val+K_MINIMAL_STACK_VAL];		\
	    tcb_t name = {															\
				.stack_base = &stack_##name[0],									\
				.stack_size	= K_MINIMAL_STACK_VAL+stack_size_val,				\
				.thread_prio=priority,											\
				.thread_wait=0,													\
				.created=false,													\
				.wake_tick=0,													\
		}



/**
 *  @fn thread_create()
 *  @brief installs a thread and put it on ready list
 *
 *  @param func - thread entry point
 *  @param arg - custom argument to pass to the thread after its creation
 *  @param tcb - previously created thread control block NOTE: cannot be NULL!
 *
 *  @return k_status_ok or error code in case of invalid value
 */
k_status_t thread_create(thread_t func, void *arg,tcb_t *tcb);


/**
 *  @fn thread_abort()
 *  @brief stops thread execution and make it not executable again (only using create)
 *
 *  @param t - thread to be stopped, if NULL is passed the current thread is aborted
 *
 *  @return k_status_ok or error code in case of invalid value
 */
k_status_t thread_abort(tcb_t *t);


/**
 *  @fn thread_suspend()
 *  @brief suspends thread execution until a thread_resume is invoked
 *
 *  @param t - thread to be suspended, if NULL passed, it suspends the current thread
 *
 *  @return k_status_ok or error code in case of invalid value
 *
 *  NOTE: thread suspend does not nest successive calls, so event if suspend was called
 *        multiple times, the thread will be woken if resume is called once
 *
 */
k_status_t thread_suspend(tcb_t *t);


/**
 *  @fn thread_resume()
 *  @brief resume a previous suspended thread execution and places it on ready list
 *
 *  @param  t - thread to be resumed
 *
 *  @return k_status_ok or error code in case of invalid value
 *
 */
k_status_t thread_resume(tcb_t *t);


/**
 *  @fn thread_wait_signals()
 *  @brief suspend a thread exeecution and wait for a single or combination of signals
 *
 *  @param t - thread to wait signals, if NULL is passed suspend the current thread
 *  @param signals - single or a bitmask of signals to be awaited
 *  @param opt - thread will block and woke up if:
 *  				@k_wait_match_pattern - woke only if receive the same signals combined
 *					@Wk_wait_match_any - woke if at least 1 signal is asserted
 *					@k_match_pattern_consume - woke by a combination and consume the signals
 *					@k_match_any_consume - woke at lest 1 signal assertion and consume it
 *
 *  @param err - pointer to a error variable to receive k_status_ok or error code
 *
 *  @return signals asserted if this task was woken
 */
uint32_t thread_wait_signals(tcb_t *t, uint32_t signals, thread_signal_opt_t opt, k_status_t *err);



/**
 *  @fn thread_set_signals()
 *  @brief set specific signals for a thread
 *
 *  @param t - thread which wait signals
 *  @param signals - signals to set can be  bitmask of combined signals
 *
 *  @return k_status_ok or error code in case of invalid value
 */
k_status_t thread_set_signals(tcb_t *t, uint32_t signals);

/**
 *  @fn thread_clr_signals()
 *  @brief clear specific signals for a thread
 *
 *  @param t - thread which wait signals or need to consume it
 *  @param signals - signals to clear can be  bitmask of combined signals
 *
 *  @return k_status_ok or error code in case of invalid value
 */
k_status_t thread_clr_signals(tcb_t *t, uint32_t signals);


/**
 *  @fn thread_yield()
 *  @brief send task to the back of ready list allowing another thread that shares priority level to run

 *  @param none
 *
 *  @return k_status_ok or error code in case of invalid value
 *
 *  NOTE: Different of k_yield() function this task will perform the yielding and if
 *        needed it will perform automatic reescheduling
 *
 */
k_status_t thread_yield(void);


/**
 *  @fn thread_set_prio()
 *  @brief change the priority of a thread in runtime
 *
 *  @param t - thread who wants to change its priority, if NULL changes the priority of current thread
 *  @param prio - new priority value ranged from 0 to 31
 *
 *  @return k_status_ok or error code in case of invalid value
 *
 *  NOTE: After to call this routine the preemption of the kernel will
 *        be triggered, so if the priority of current thread reduces and a new
 *        highest priority is found by scheduler the current thread will be
 *        suspended immediately until this new highest priority task blocks.
 *        In same form if the priority of a ready but not running task arises
 *        in form to be the new highest priority ready to run, the caller
 *        of this function is suspended immediatelly and the changed priority
 *        thread is placed on execution
 */
k_status_t thread_set_prio(tcb_t *t, uint8_t prio);


/**
 *  @fn thread_get_current()
 *  @brief gets the current running thread id
 *
 *  @param none
 *
 *  @return tcb pointer to the current thread id
 */
tcb_t *thread_get_current(void);


#endif
