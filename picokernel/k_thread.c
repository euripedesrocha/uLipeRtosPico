/**
 * 							ULIPE RTOS PICO
 *
 *  @file k_thread.c
 *
 *  @brief threading managament core file
 *
 *
 */

#include "ulipe_rtos_pico.h"

/** public variables */
extern tcb_t *k_current_task;


/** static functions */

/**
 *  @fn thread_handle_signal_act()
 *  @brief wakes a waiting task which meet the signals conditions
 *  @param
 *  @return
 */
static bool thread_handle_signal_act(tcb_t *t)
{
	bool match = false;


	if(t->thread_wait & (K_THR_PEND_SIGNAL_ANY |  K_THR_PEND_SIGNAL_ANY_C)){
		/* just a signal that matches with this is sufficient to wake task */
		if((t->signals_actual) & (t->signals_wait))
			match = true;
	}

	if(t->thread_wait & (K_THR_PEND_SIGNAL_ALL |  K_THR_PEND_SIGNAL_ALL_C)){
		/* here the mask must be the same */
		if((t->signals_actual) == (t->signals_wait))
			match = true;
	}


	if(match == true){
		if(t->thread_wait & (K_THR_PEND_SIGNAL_ALL_C | K_THR_PEND_SIGNAL_ANY_C ))
			t->signals_actual = 0;
		/* on a match, clear the signal waiting flags */
		t->signals_wait = 0;
		t->thread_wait &= ~(K_THR_PEND_SIGNAL_ANY |  K_THR_PEND_SIGNAL_ALL|
				K_THR_PEND_SIGNAL_ALL_C | K_THR_PEND_SIGNAL_ANY_C);
	}

	return(match);
}


/**
 *  @fn thread_handle_signal_wait()
 *  @brief put task in wait state that matches user options about signals to wait
 *  @param
 *  @return
 */
static bool thread_handle_signal_wait(tcb_t *t, uint32_t wait_type, archtype_t signals)
{
	bool match = false;

	t->thread_wait |= wait_type;
	t->signals_wait = signals;

	/* evaluate if signals is aready asserted */
	match = thread_handle_signal_act(t);

	return(match);

}



/** public fnctions */

k_status_t thread_create(thread_t func, void *arg,tcb_t *tcb)
{
	k_status_t ret = k_status_ok;

	/* thread create must not be called from isr,
	 * thread func must be valid and tcb as well
	 */
	if(port_from_isr()){
		ret = k_status_illegal_from_isr;
		goto cleanup;
	}

	if(func == NULL) {
		ret = k_status_invalid_param;
		goto cleanup;
	}

	if(tcb == NULL) {
		ret = k_status_invalid_param;
		goto cleanup;
	}


	if(k_current_task == tcb) {
		/* cannot re-create the current running task */
		ret = k_status_invalid_param;
		goto cleanup;
	}

	/*
	 * The contents of tcb must represent a initialized one
	 * allocated with THREAD_CONTROL_BLOCK_DECLARE() otherwise
	 * the thread will not created
	 */
	if((tcb->thread_prio > (K_PRIORITY_LEVELS - 1)) ||
			(tcb->thread_prio < (-(K_PRIORITY_LEVELS - 1)))){
		ret = k_status_invalid_param;
		goto cleanup;
	}


	if(tcb->stack_size < K_MINIMAL_STACK_VAL) {
		ret = k_status_invalid_param;
		goto cleanup;
	}

	if(tcb->stack_base == NULL) {
		ret = k_status_invalid_param;
		goto cleanup;
	}

	/* disable scheduling durint task creation */
	archtype_t key = port_irq_lock();

	tcb->signals_wait = 0;
	tcb->thread_wait = 0;
	tcb->timer_wait = 0;
	tcb->created = true;
	sys_dlist_init(&tcb->thr_link);

	/* initialize stack contents */
	tcb->stack_top = port_create_stack_frame(tcb->stack_base + (archtype_t)tcb->stack_size, func, arg);
	ULIPE_ASSERT(tcb->stack_top != NULL);


	/* insert the created thread on ready list */
	ret=k_make_ready(tcb);
	ULIPE_ASSERT(ret == k_status_ok);


	/* allow kernel to run, perform reeschedule */
	port_irq_unlock(key);
	k_sched_and_swap();
	ret = k_status_ok;

cleanup:
	return(ret);
}

k_status_t thread_abort(tcb_t *t)
{
	k_status_t ret = k_status_ok;

	/* disable scheduling during task abortion */
	archtype_t key = port_irq_lock();

	if(t == NULL) {
		/* null thread can be the current */
		t = k_current_task;
		ULIPE_ASSERT(t!= NULL);
	}


	/* this is a ready task so, remove it from ready list */
	ret=k_make_not_ready(t);
	ULIPE_ASSERT(ret == k_status_ok);

	/* de init the thread  but keeps some parameters
	 * allowing user to re-create it if necessary
	 */
	t->created = false;


	/* perform reesched to find next task to run */
	port_irq_unlock(key);
	k_sched_and_swap();
	ret = k_status_ok;

	return(ret);
}

k_status_t thread_suspend(tcb_t *t)
{
	k_status_t ret = k_status_ok;

	if(t->thread_wait & K_THR_SUSPENDED) {
		/* thread is already suspended */
		ret = k_thread_susp;
		goto cleanup;
	}

	if(port_from_isr()){
		/* suspend cannot be called from ISR */
		ret = k_status_illegal_from_isr;
		goto cleanup;
	}

	archtype_t key = port_irq_lock();

	if(t == NULL) {
		/* null thread can be the current */
		t = k_current_task;
		ULIPE_ASSERT(t!= NULL);
	}


	/* to suspend a thread, only remove it from ready list and
	 * mark with suspended
	 */
	ret=k_make_not_ready(t);
	ULIPE_ASSERT(ret == k_status_ok);

	t->thread_wait |= K_THR_SUSPENDED;


	/* perform reesched to find next task to run */
	port_irq_unlock(key);
	k_sched_and_swap();
	ret = k_status_ok;


cleanup:
	return(ret);
}

k_status_t thread_resume(tcb_t *t)
{
	k_status_t ret = k_status_ok;

	if(t == NULL) {
		ret = k_status_invalid_param;
		goto cleanup;
	}

	if((t->thread_wait & K_THR_SUSPENDED) == 0) {
		/* thread is not suspended */
		ret = k_status_invalid_param;
		goto cleanup;
	}


	archtype_t key = port_irq_lock();
	t->thread_wait &= ~(K_THR_SUSPENDED);

	ret=k_make_ready(t);
	ULIPE_ASSERT(ret == k_status_ok);

	/* perform reesched to find next task to run */
	port_irq_unlock(key);
	k_sched_and_swap();
	ret = k_status_ok;

cleanup:
	return(ret);
}

uint32_t thread_wait_signals(tcb_t *t, uint32_t signals, thread_signal_opt_t opt, k_status_t *err)
{
	uint32_t rcvd = 0xFFFFFFFF;
	bool match = false;
	k_status_t ret;


	if(t == NULL) {
		/* null thread can be the current */
		t = k_current_task;
		ULIPE_ASSERT(t!= NULL);
	}



	if(port_from_isr()){
		/* wait cannot be called from ISR */
		ret = k_status_illegal_from_isr;
		goto cleanup;
	}

	archtype_t key = port_irq_lock();


	/*
	 * Select and prepare task to wait for the conditions imposed to signaling
	 */
	switch(opt) {
		case k_wait_match_pattern:
			match = thread_handle_signal_wait(t, K_THR_PEND_SIGNAL_ALL, signals);
		break;

		case k_wait_match_any:
			match = thread_handle_signal_wait(t, K_THR_PEND_SIGNAL_ANY, signals);
		break;

		case k_match_pattern_consume:
			match = thread_handle_signal_wait(t, K_THR_PEND_SIGNAL_ALL_C, signals);
		break;

		case k_match_any_consume:
			match = thread_handle_signal_wait(t, K_THR_PEND_SIGNAL_ANY_C, signals);
		break;

		default:
			ret = k_status_invalid_param;
			port_irq_unlock(key);
			goto cleanup;
	}



	/* if signals ware already asserted theres no need to reesched */
	if(match) {
		rcvd = t->signals_actual;
		port_irq_unlock(key);
		goto cleanup;
	}

	ret = k_make_not_ready(t);
	ULIPE_ASSERT(ret == k_status_ok);


	port_irq_unlock(key);
	/* perform reesched to find next task to run */
	k_sched_and_swap();
	ret = k_status_ok;

	rcvd = t->signals_actual;

cleanup:
	if(err != NULL)
		*err = ret;

	return(rcvd);
}

k_status_t thread_set_signals(tcb_t *t, uint32_t signals)
{
	k_status_t ret = k_status_ok;
	bool match =false;

	if(t == NULL) {
		ret = k_status_invalid_param;
		goto cleanup;
	}

	archtype_t key = port_irq_lock();

	t->signals_actual |= signals;
	match = thread_handle_signal_act(t);


	if(!match) {
		port_irq_unlock(key);
		goto cleanup;
	}


	ret = k_make_ready(t);
	ULIPE_ASSERT(ret == k_status_ok);
	port_irq_unlock(key);


	/* perform reesched to find next task to run */
	k_sched_and_swap();
	ret = k_status_ok;


cleanup:
	return(ret);
}

k_status_t thread_clr_signals(tcb_t *t, uint32_t signals)
{

	k_status_t ret = k_status_ok;
	bool match =false;

	if(t == NULL) {
		ret = k_status_invalid_param;
		goto cleanup;
	}

	archtype_t key = port_irq_lock();

	t->signals_actual &= ~signals;
	match = thread_handle_signal_act(t);



	if(!match) {
		port_irq_unlock(key);
		goto cleanup;
	}

	ret = k_make_ready(t);
	ULIPE_ASSERT(ret == k_status_ok);
	port_irq_unlock(key);


	/* perform reesched to find next task to run */
	k_sched_and_swap();
	ret = k_status_ok;

cleanup:
	return(ret);
}

k_status_t thread_yield(void)
{
	k_status_t ret = k_status_ok;
	bool resched = false;

	archtype_t key = port_irq_lock();
	tcb_t *t = k_current_task;


	/*
	 * Yielding a task is very simple, we force a fifo remotion, then
	 * pick the removed entry and send back to the fifo, the kernel
	 * scheduling deal with case that only thread as the top priority and try to yield
	 * itself
	 */
	resched = k_yield(t);

	if(!resched) {
		port_irq_unlock(key);
		goto cleanup;
	}

	port_irq_unlock(key);
	k_sched_and_swap();

cleanup:
	return(ret);
}


k_status_t thread_set_prio(tcb_t *t, int8_t prio)
{
	k_status_t ret = k_status_ok;

	if((prio > (K_PRIORITY_LEVELS - 1)) ||
			(prio < (-1 * (K_PRIORITY_LEVELS - 1)))){
		ret = k_status_invalid_param;
		goto cleanup;
	}

	archtype_t key = port_irq_lock();


	if(t == NULL) {
		/* null thread can be the current */
		t = k_current_task;
		ULIPE_ASSERT(t!= NULL);
	}



	if(t->thread_prio == prio) {
		/* same prio, no need to process it */
		ret = k_status_invalid_param;
		port_irq_unlock(key);
		goto cleanup;
	}




	/* the priority change impacts on moving the task to another level
	 * on ready fifo
	 */
	if(!t->thread_wait) {
		ret = k_make_not_ready(t);
		ULIPE_ASSERT(ret == k_status_ok);
	}

	t->thread_prio = prio;


	if(t->thread_wait) {
		goto cleanup;
	}

	ret = k_make_ready(t);
	ULIPE_ASSERT(ret == k_status_ok);
	port_irq_unlock(key);

	k_sched_and_swap();
	ret = k_status_ok;


cleanup:
	return(ret);
}

tcb_t *thread_get_current(void)
{
	tcb_t *ret;
	ret = k_current_task;
	return(ret);
}
