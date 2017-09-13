/**
 * 							ULIPE RTOS PICO
 *
 *  @file k_mutex.h
 *
 *  @brief mutual exclusion semaphore stuff
 *
 */
#include "ulipe_rtos_pico.h"


/** internal functions */


/** public functions */
k_status_t mutex_take(kmutex_t *m, bool try)
{
	k_status_t ret = k_status_ok;
	bool reesched = false;
	tcb_t *t = thread_get_current();
	ULIPE_ASSERT(t != NULL);


	if(m == NULL) {
		ret = k_status_invalid_param;
		goto cleanup;
	}

	if(port_from_isr()){
		/* take cannot be called from ISR */
		ret = k_status_illegal_from_isr;
		goto cleanup;
	}

	archtype_t key = port_irq_lock();

	if(!m->created) {
		/* handle first time usage */
		k_work_list_init(&m->threads_pending);
		m->created = true;
	}


	if(m->thr_owner == NULL && (try)) {
		port_irq_unlock(key);
		goto cleanup;
	}

	if(m->thr_owner != NULL ) {
		/*
		 * if no key available, we need to insert the waiting thread
		 * on mutex pending list, when a key will become available
		 * the task will be woken as well
		 */

		ret = k_make_not_ready(t);
		ULIPE_ASSERT(ret == k_status_ok);
		t->thread_wait |= K_THR_PEND_MTX;
		ret = k_pend_obj(t, &m->threads_pending);
		ULIPE_ASSERT(ret == k_status_ok);

		/* but in mutex case, if the owner has a priority
		 * too low, raise it with the priority of current task
		 */
		reesched = true;

	} else {

		m->thr_owner = t;
		m->owner_prio = t->thread_prio;
		thread_set_prio(t, K_MUTEX_PRIO_CEIL_VAL);
	}


	if(!reesched){
		port_irq_unlock(key);
		goto cleanup;
	}

	port_irq_unlock(key);
	/*
	 * if current thread entered on pending state, we need to reesched the
	 * thread set and find a new thread to execute, otherwise, dispatch idle
	 */
	k_sched_and_swap();

cleanup:
	return(ret);
}



k_status_t mutex_give(kmutex_t *m)
{
	k_status_t ret = k_status_ok;
	tcb_t *t = NULL;
	tcb_t *cur = thread_get_current();

	if(port_from_isr()){
		/* take cannot be called from ISR */
		ret = k_status_illegal_from_isr;
		goto cleanup;
	}


	if(m == NULL) {
		ret = k_status_invalid_param;
		goto cleanup;
	}


	if(cur != m->thr_owner) {
		/* only the mutex owner can release it */
		ret = k_status_invalid_param;
		goto cleanup;
	}

	if(m->thr_owner == NULL) {
		ret = k_mutex_already_available;
		goto cleanup;

	}

	archtype_t key = port_irq_lock();
	if(!m->created) {
		/* handle first time usage */
		k_work_list_init(&m->threads_pending);
		m->created = true;
	}

	/* restore thread original priority */

	/*
	 * once a mutex was updated its keys
	 * we need to verify if a new highprio task is available to
	 * run
	 */
	t = k_unpend_obj(&m->threads_pending);
	if(t == NULL) {
		/* no tasks pendings, just get out here */
		uint8_t tmp = m->owner_prio;
		m->owner_prio = 0;
		m->thr_owner  = NULL;

		/* restore thread original priority */
		thread_set_prio(cur, tmp);

		port_irq_unlock(key);
		goto cleanup;
	} else {
		uint8_t tmp = m->owner_prio;
		m->thr_owner = t;
		m->owner_prio = t->thread_prio;
		t->thread_wait &= ~(K_THR_PEND_MTX);

		ret = k_make_ready(t);
		ULIPE_ASSERT(ret == k_status_ok);
		/*restore last owner original prio */
		thread_set_prio(cur, tmp);


		port_irq_unlock(key);


		k_sched_and_swap();
	}


cleanup:
	return(ret);
}
