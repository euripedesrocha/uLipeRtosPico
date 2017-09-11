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


/* define the stask status */
#define K_THR_SUSPENDED  			(0x01)
#define K_THR_PEND_SEMA  			(0x02)
#define K_THR_PEND_MSG	 			(0x04)
#define K_THR_PEND_TMR	 			(0x08)
#define K_THR_PEND_SIGNAL_ALL 		(0x10)
#define K_THR_PEND_SIGNAL_ANY 		(0x20)
#define K_THR_PEND_SIGNAL_ALL_C 	(0x40)
#define K_THR_PEND_SIGNAL_ANY_C 	(0x80)
#define K_THR_PEND_TICKER	 		(0x100)



/* define signals options */
typedef enum {
	k_wait_match_pattern = 0,
	k_wait_match_any,
	k_match_pattern_consume,
	k_match_any_consume,
}thread_signal_opt_t;

/* thread function definition */
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
 *  @brief declares a full initialized thread control block
 *  @param
 *  @return
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
 *  @param
 *  @return
 */
k_status_t thread_create(thread_t func, void *arg,tcb_t *tcb);

/**
 *  @fn thread_abort()
 *  @brief stops thread execution and make it not executable again (only using create)
 *  @param
 *  @return
 */
k_status_t thread_abort(tcb_t *t);

/**
 *  @fn thread_suspend()
 *  @brief suspends thread execution until a thread_resume is invoked
 *  @param
 *  @return
 */
k_status_t thread_suspend(tcb_t *t);


/**
 *  @fn thread_resume()
 *  @brief resume a previous suspended thread execution and places it on ready list
 *  @param
 *  @return
 */
k_status_t thread_resume(tcb_t *t);

/**
 *  @fn thread_wait_signals()
 *  @brief suspend a thread exeecution and wait for a signal pattern
 *  @param
 *  @return
 */
uint32_t thread_wait_signals(tcb_t *t, uint32_t signals, thread_signal_opt_t opt, k_status_t *err);


/**
 *  @fn thread_set_signals()
 *  @brief set specific signals for a thread
 *  @param
 *  @return
 */
k_status_t thread_set_signals(tcb_t *t, uint32_t signals);

/**
 *  @fn thread_clr_signals()
 *  @brief clear specific signals for a thread
 *  @param
 *  @return
 */
k_status_t thread_clr_signals(tcb_t *t, uint32_t signals);


/**
 *  @fn thread_yield()
 *  @brief send task to the back of ready list allowing another thread that shares priority level to run
 *  @param
 *  @return
 */
k_status_t thread_yield(void);

/**
 *  @fn thread_set_prio()
 *  @brief change the priority of a thread in runtime
 *  @param
 *  @return
 */
k_status_t thread_set_prio(tcb_t *t, int8_t prio);

/**
 *  @fn thread_get_current()
 *  @brief gets the current running thread id
 *  @param
 *  @return
 */
tcb_t *thread_get_current(void);






#endif
