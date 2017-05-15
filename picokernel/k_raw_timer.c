/**
 * 							ULIPE RTOS PICO
 *  @file k_raw_timer.h
 *
 *  @brief timer primitive kernel header file
 *
 */

#include "ulipe_rtos_pico.h"


#if(K_ENABLE_TIMERS > 0)

#define K_TIMER_NO_WAKEUP_TASK (archtype_t)0xFFFFFFFF

/* static variables */
static k_list_t k_timed_list;
static archtype_t k_next_wakeup_value = K_TIMER_NO_WAKEUP_TASK;
THREAD_CONTROL_BLOCK_DECLARE(timer_tcb, K_TIMER_DISPATCHER_STACK_SIZE, K_TIMER_DISPATCHER_PRIORITY);



/** private functions */


/**
 *  @fn timer_period_sort()
 *  @brief	sort all active timers on list to find which has the least time to wait
 *  @param
 *  @return
 */
static ktimer_t *timer_period_sort(k_list_t *tlist)
{
	ktimer_t *ret = NULL;
	return(ret);
}

/**
 *  @fn timer_rebuild_timeline()
 *  @brief	rebuild the timer dispatch scheduling 
 *  @param
 *  @return
 */
static void timer_rebuild_timeline(ktimer_t *t)
{

}

static bool timer_remove_from_timeline(ktimer_t *t)
{
	bool ret = false;

	return(ret);
}

/**
 *  @fn timer_dispatcher()
 *  @brief	thread which dispatches services when expiration occur
 *  @param
 *  @return
 */
static void timer_dispatcher(void *args)
{
	(void)args;

	for(;;){

	}
}



/** public functions */

k_status_t timer_start(ktimer_t *t)
{
	k_status_t ret = k_status_ok;
	archtype_t key;

	if(t == NULL) {
		ret = k_status_invalid_param;
		goto cleanup;
	}

	if(!t->load_val) {
		/* timer not loaded cannot start */
		ret = k_status_invalid_param;
		goto cleanup;
	}


	key = port_irq_lock();

	if(!t->created) {
		t->created = true;
		k_work_list_init(&t->threads_pending);
		sys_dlist_init(&t->timer_list_link);

	}


	t->expired = false;
	/* new valid timer added to list 
	 * insert it on timeline
	 */
	timer_rebuild_timeline(t);

	port_irq_unlock(key);

cleanup:
	return(ret);

}



k_status_t timer_poll(ktimer_t *t)
{
	k_status_t ret = k_status_ok;
	archtype_t key;

	if(t == NULL) {
		ret = k_status_invalid_param;
		goto cleanup;
	}

	key = port_irq_lock();

	if(!t->created) {
		t->created = true;
		k_work_list_init(&t->threads_pending);
		sys_dlist_init(&t->timer_list_link);
	}


	/* only one thread per timer can poll */
	if(t->threads_pending.bitmap) {
		ret = k_timer_busy;
		port_irq_unlock(key);
		goto cleanup;
	}

	/* check if timer is already expired */
	if(t->expired){
		ret = k_timer_expired;
		port_irq_unlock(key);
		goto cleanup;
	}	

	/* 
	 * If it safe to poll timer, sleep thread
	 * until the dispatcher wakes it up.
	 */


	tcb_t *thr = thread_get_current();
	ULIPE_ASSERT(thr != NULL);

	ret = k_make_not_ready(thr);
	ULIPE_ASSERT(ret == k_status_ok);
	thr->thread_wait |= K_THR_PEND_TMR;

	ret = k_pend_obj(thr, &t->threads_pending);
	ULIPE_ASSERT(ret == k_status_ok);
	port_irq_unlock(key);

	/* reescheduling is needed */
	ret = k_sched_and_swap();
	ULIPE_ASSERT(ret == k_status_ok);	

cleanup:
	return(ret);

}



k_status_t timer_set_callback(ktimer_t *t, ktimer_callback_t cb)
{
	k_status_t ret = k_status_ok;
	archtype_t key;

	if(t == NULL) {
		ret = k_status_invalid_param;
		goto cleanup;
	}

	if(cb == NULL) {
		ret = k_status_invalid_param;
		goto cleanup;		
	}

	key = port_irq_lock();

	if(!t->created) {
		t->created = true;
		k_work_list_init(&t->threads_pending);
		sys_dlist_init(&t->timer_list_link);

	}


	if(t->expired) {
		ret = k_timer_expired
		port_irq_unlock(key);
		goto cleanup;
	}

	if(k_next_wakeup_value != K_TIMER_NO_WAKEUP_TASK)
		port_timer_halt();

	t->cb = cb;

	if(k_next_wakeup_value != K_TIMER_NO_WAKEUP_TASK)
		port_timer_resume();

	port_irq_unlock(key);

cleanup:
	return(ret);
}


k_status_t timer_set_load(ktimer_t *t, archtype_t load_val)
{
	k_status_t ret = k_status_ok;
	archtype_t key;

	if(t == NULL) {
		ret = k_status_invalid_param;
		goto cleanup;
	}

	key = port_irq_lock();

	if(!t->created) {
		t->created = true;
		k_work_list_init(&t->threads_pending);
		sys_dlist_init(&t->timer_list_link);

	}

	if(t->running){
		/* cannot set load value of a running timer */
		ret = k_timer_running;
		port_irq_unlock(key);
		goto cleanup;
	}

	if(k_next_wakeup_value != K_TIMER_NO_WAKEUP_TASK)
		port_timer_halt();

	t->load_val = load_val;

	if(k_next_wakeup_value != K_TIMER_NO_WAKEUP_TASK)
		port_timer_resume();

	port_irq_unlock(key);

cleanup:
	return(ret);
}


k_status_t timer_stop(ktimer_t *t)
{
	k_status_t ret = k_status_ok;
	archtype_t key;
	bool reesched = false;

	if(t == NULL) {
		ret = k_status_invalid_param;
		goto cleanup;
	}

	key = port_irq_lock();

	if(!t->created) {
		t->created = true;
		k_work_list_init(&t->threads_pending);
		sys_dlist_init(&t->timer_list_link);		
	}

	if(!t->running){
		/* cannot set load value of a running timer */
		ret = k_timer_stopped;
		port_irq_unlock(key);
		goto cleanup;
	}

	reesched = timer_remove_from_timeline(t);
	if(!reesched) {
		port_irq_unlock(key);
		goto cleanup;
	}
	port_irq_unlock(key);

	ret = k_sched_and_swap();
	ULIPE_ASSERT(ret == k_status_ok);

cleanup:
	return(ret);
}

#endif