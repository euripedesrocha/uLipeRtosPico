/**
 * 							ULIPE RTOS PICO
 *  @file k_raw_timer.h
 *
 *  @brief timer primitive kernel header file
 *
 */

#include "ulipe_rtos_pico.h"

#if (K_ENABLE_TICKER > 0)

#define K_TIMER_NO_WAKEUP_TASK (archtype_t)0xFFFFFFFF


#if(K_ENABLE_TIMERS > 0)
/* static variables */
static k_list_t k_timed_list;
static archtype_t k_elapsed_time = 0;
static bool no_timers = true;
#endif

uint32_t tick_count = 0;
tcb_t * next_task_wake = NULL;
static k_list_t k_ticker_list;


THREAD_CONTROL_BLOCK_DECLARE(timer_tcb, K_TIMER_DISPATCHER_STACK_SIZE, K_TIMER_DISPATCHER_PRIORITY);

/** private functions */
/**
 *  @fn timer_period_sort()
 *  @brief	sort all active timers on list to find which has the least time to wait
 *  @param
 *  @return
 */
static void timer_step_tick(void) 
{
	k_list_t *head;
	tcb_t *ret = NULL;

	tick_count++;
	/* no timer to run */
	if(sys_dlist_is_empty(&k_ticker_list)) {
		goto cleanup;
	}

	/* walk to list and check if task needs to be woken */
	SYS_DLIST_FOR_EACH_CONTAINER(&k_ticker_list, ret, thr_link) {
		if((next_task_wake->wake_tick >= ret->wake_tick) && (ret->wake_tick != 0)) {
			next_task_wake = ret;
		}
	}

cleanup:
	return;
}

/**
 *  @fn timer_period_sort()
 *  @brief	sort all active timers on list to find which has the least time to wait
 *  @param
 *  @return
 */

#if(K_ENABLE_TIMERS > 0)
 
static ktimer_t *timer_period_sort(k_list_t *tlist)
{
	ktimer_t *ret = NULL;
	ktimer_t *tmp = NULL;
	k_list_t *head;

	if(sys_dlist_is_empty(tlist))
		goto cleanup;

	/*
	 * The objective here is to find the 
	 * timer which loads the less valued
	 * load to do it iterate list and compare each
	 * time against with reference, pick and keeps
	 * ret with this timer until interation ends
	 * or another load value fits in this rule
	 */
	head = sys_dlist_peek_head(tlist);
	ULIPE_ASSERT(head != NULL);	
	ret = CONTAINER_OF(head, ktimer_t, timer_list_link);
	ULIPE_ASSERT(ret != NULL);	

	SYS_DLIST_FOR_EACH_CONTAINER(tlist, tmp, timer_list_link) {
		if(ret->load_val > tmp->load_val) {
			ret = tmp;
		}
	}

cleanup:
	return(ret);
}

/**
 *  @fn timer_rebuild_timeline()
 *  @brief	rebuild the timer dispatch scheduling 
 *  @param
 *  @return
 */
static void timer_rebuild_timeline(ktimer_t *t, archtype_t *key)
{
	ULIPE_ASSERT(t != NULL);
	ULIPE_ASSERT(key != NULL);
	archtype_t cmd;


	/* put the new timer on timeline list */
	sys_dlist_append(&k_timed_list, &t->timer_list_link);


	/* check if timer is not running */
	if(no_timers) {
		cmd = K_TIMER_LOAD_FRESH;

		t->load_val = t->timer_to_wait;
		t->running  = true;
		t->expired  = false;


		
	} else {
		cmd = K_TIMER_REFRESH;
		/* memorize the point of timeline when timer was added, we
		 * will use it to calculate the amount of counts
		 * to be loaded on timer IP when this timer 
		 * will be selected
		 */
			uint32_t tim = port_timer_halt();


			t->load_val = tim + t->timer_to_wait;
			t->running  = true;
			t->expired  = false;

	}
	port_irq_unlock(*key);
	thread_set_signals(&timer_tcb,cmd);
	*key = port_irq_lock();

}
#endif

/**
 *  @fn timer_dispatcher()
 *  @brief	thread which dispatches services when expiration occur
 *  @param
 *  @return
 */
void timer_dispatcher(void *args)
{
	(void)args;
	k_status_t err;
	ktimer_t *actual_timer;
	archtype_t signals = 0, clear_msk =0;

#if(K_ENABLE_TIMERS > 0)
	archtype_t key = port_irq_lock();
	sys_dlist_init(&k_timed_list);
	actual_timer = NULL;
	port_irq_unlock(key);
#endif

	for(;;){
		/* The dispatcher manages the incoming commands for kernel timer, 
		 * when the timer is not running and a fresh load was issued, 	
		 * the timer places the first point of timelinte. when timer expires
		 * the dispatcher is invoked and perform handling of current timer
		 * either dispatching or waking up a waiting thread (which called)
		 * a timer poll, and after sorts the timer list for the next wakeup
		 */
		if(!signals) {
			signals = thread_wait_signals(NULL, K_TIMER_LOAD_FRESH | K_TIMER_DISPATCH | K_TIMER_REFRESH, 
					k_wait_match_any, &err);
			ULIPE_ASSERT(err != k_status_error);

		}

		if(signals & K_TIMER_TICK) {
			signals &= ~(K_TIMER_TICK);
			clear_msk |= K_TIMER_TICK;

			/* check if exist some thread to wake */
			archtype_t key = port_irq_lock();
			timer_step_tick();


			/* check if time to wakeup */
			if(tick_count >= next_task_wake->wake_tick) {
				/* put this thread on ready list making it ready*/
				next_task_wake->thread_wait &= ~(THR_PEND_TICKER);
				sys_dlist_remove(&next_task_wake->thr_link);
				k_status_t err = k_make_ready(thr);
				ULIPE_ASSERT(err == k_status_ok);
			}


			/* get the next task */
			k_list_t *head = sys_dlist_peek_head(k_ticker_list);
			ULIPE_ASSERT(head != NULL);	
			
			/* no need to sort the nearest task 
			 * which will wakeup will found on next
			 * tick 
			 */
			next_task_wake = CONTAINER_OF(head, tcb_t, thr_link);

			port_irq_unlock(key);

		}

#if(K_ENABLE_TIMERS > 0)		
		/* select commands with priority */
		if(signals & K_TIMER_DISPATCH) {
			signals &= ~(K_TIMER_DISPATCH);
			clear_msk |= K_TIMER_DISPATCH;
			port_timer_halt();

			/*
			 * we know how timer needs to be woken
			 */
			if(actual_timer->cb)
				actual_timer->cb(actual_timer);

			/*
			 * thread waiting for it adds to ready list 
			 */
			if(actual_timer->threads_pending.bitmap != 0) {
				key = port_irq_lock();
				tcb_t *thr = k_unpend_obj(&actual_timer->threads_pending);
				ULIPE_ASSERT(thr != NULL);

				thr->thread_wait &= ~(K_THR_PEND_TMR);

				k_status_t err = k_make_ready(thr);
				ULIPE_ASSERT(err == k_status_ok);

				/* no need to perform schedule, when timer thread 
				 * releases the cpu this will automatically done
				 */
				port_irq_unlock(key);
			}

			key = port_irq_lock();

			/* drops the current timer */
			sys_dlist_remove(&actual_timer->timer_list_link);
			actual_timer->running = false;
			actual_timer->expired = true;

			port_irq_unlock(key);

			/* find next timer to pend */
			actual_timer= timer_period_sort(&k_timed_list);
			if(actual_timer == NULL) {
				/* no timers to run */
				no_timers = true;
			} else {
				port_timer_load_append(actual_timer->load_val);
				port_timer_resume();
			}

		}


		if(signals & K_TIMER_REFRESH) {
			signals &= ~(K_TIMER_REFRESH);
			clear_msk |= K_TIMER_REFRESH;
			no_timers = false;

			port_timer_halt();
			key = port_irq_lock();


			/* iterate list and schedule a new timer on timeline */
			ktimer_t *tmp = timer_period_sort(&k_timed_list);
			if(tmp != actual_timer) {
				actual_timer = tmp;
				port_timer_load_append(actual_timer->load_val);

			}
			port_irq_unlock(key);
			port_timer_resume();

		}

		if(signals & K_TIMER_LOAD_FRESH) {
			signals &= ~(K_TIMER_LOAD_FRESH);
			clear_msk |= K_TIMER_LOAD_FRESH;
			k_list_t *head;
			no_timers = false;

			key = port_irq_lock();
			/* gets the only available container of timed list */
			head = sys_dlist_peek_head(&k_timed_list);
			ULIPE_ASSERT(head != NULL);	
			actual_timer = CONTAINER_OF(head, ktimer_t, timer_list_link);
			port_irq_unlock(key);


			/* no timers running so put a new match value */
			port_start_timer(actual_timer->load_val);
			/* start the timeline running */
			no_timers = false;
		}
#endif
		thread_clr_signals(&timer_tcb, clear_msk);
		clear_msk = 0;

	}
}



/** public functions */
#if(K_ENABLE_TIMERS > 0)

k_status_t timer_start(ktimer_t *t)
{
	k_status_t ret = k_status_ok;
	archtype_t key;

	if(t == NULL) {
		ret = k_status_invalid_param;
		goto cleanup;
	}
	
	if(t->running){
		ret = k_timer_running;
		goto cleanup;
	}

	if(!t->timer_to_wait) {
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
	timer_rebuild_timeline(t, &key);

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

	if(!t->running) {
		/* timer must be running */
		ret = k_timer_stopped;
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
		ret = k_timer_expired;
		port_irq_unlock(key);
		goto cleanup;
	}

	t->cb = cb;

	port_irq_unlock(key);

cleanup:
	return(ret);
}


k_status_t timer_set_load(ktimer_t *t, uint32_t load_val)
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

	t->timer_to_wait = load_val;

	port_irq_unlock(key);

cleanup:
	return(ret);
}

#endif


k_status_t ticker_timer_wait(uint32_t ticks)
{
	k_status_t ret = k_status_ok;
	if(!ticks) {
		ret = k_status_invalid_param
		return(ret);
	}

	tcb_t *thr = thread_get_current();
	ULIPE_ASSERT(t != NULL);

	archtype_t key = port_irq_lock();

	ret = k_make_not_ready(thr);
	ULIPE_ASSERT(ret == k_status_ok);
	thr->wake_tick = tick_count + ticks;
	thr->thread_wait |= THR_PEND_TICKER;

	/* put the new timer on ticker list */
	sys_dlist_append(&k_ticker_list, &thr->thr_link);

	port_irq_unlock(key);
	/* reescheduling is needed */
	ret = k_sched_and_swap();
	ULIPE_ASSERT(ret == k_status_ok);	
cleanup:
	return(ret);
}
#endif
