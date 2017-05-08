/**
 * 							ULIPE RTOS PICO
 *
 *  @file k_sema.h
 *
 *  @brief basic semaphore usage file
 *
 */
#include "ulipe_rtos_pico.h"

/* sempahore priority ceilling value */
#define K_SEMA_CEILLING_PRIO	-1

/** private functions */
static void semaphore_handle_celling(ksema_t *s, tcb_t *t)
{
	uint8_t prio = t->thread_prio;
	thread_set_prio(t, K_SEMA_CEILLING_PRIO);
	s->prio = prio;
	s->prio_ceilling = true;
	s->cnt--;
}

static void semahore_restore_prio(ksema_t *s, tcb_t *t)
{
	ULIPE_ASSERT(t != NULL);
	if(!s->prio_ceilling)
		goto cleanup;

	/*
	 * Only the thread that´s used ceilling
	 * can give back the semaphore, other
	 * cases are not allowed, traps the kernel.
	 */
	ULIPE_ASSERT(t->thread_prio == K_SEMA_CEILLING_PRIO);

	uint8_t prio = s->prio;
	s->prio = t->thread_prio;
	thread_set_prio(t, prio);
	s->prio_ceilling = false;

cleanup:
	return;
}


/** public functions */
k_status_t semaphore_take(ksema_t *s)
{
	k_status_t ret = k_status_ok;
	bool reesched = false;
	tcb_t *t = thread_get_current();
	ULIPE_ASSERT(t != NULL);


	if(s == NULL) {
		ret = k_status_invalid_param;
		goto cleanup;
	}

	if(port_from_isr()){
		/* take cannot be called from ISR */
		ret = k_status_illegal_from_isr;
		goto cleanup;
	}

	if(s->prio_ceilling) {
		/* semaphore used with priority ceilling cannot be treated as
		 * a regular semaphore again
		 */
		ret = k_sema_illegal_use_celling;
		goto cleanup;
	}

	if(!s->created) {
		/* handle first time usage */
		k_work_list_init(&s->threads_pending);
		s->created = true;
	}


	ret = k_sched_lock();
	ULIPE_ASSERT(ret == k_status_ok);

	if(!s->cnt) {
		/*
		 * if no key available, we need to insert the waiting thread
		 * on semaphore pending list, when a key will become available
		 * the task will be woken as well
		 */
		ret = k_make_not_ready(t);
		ULIPE_ASSERT(ret == k_status_ok);

		t->thread_wait |= K_THR_PEND_SEMA;

		ret = k_pend_obj(t, &s->threads_pending);
		ULIPE_ASSERT(ret == k_status_ok);

		reesched = true;
	} else {
		s->cnt--;
	}


	if(!reesched){
		ret = k_sched_unlock();
		ULIPE_ASSERT(ret == k_status_ok);
		goto cleanup;
	}


	ret = k_sched_unlock();
	ULIPE_ASSERT(ret == k_status_ok);

	/*
	 * if current thread entered on pending state, we need to reesched the
	 * thread set and find a new thread to execute, otherwise, dispatch idle
	 */
	k_sched_and_swap();

	/*
	 * thread woken, immediately give a semaphore to it
	 */
	s->cnt--;


cleanup:
	return(ret);
}

k_status_t semaphore_take_and_ceil(ksema_t *s)
{
	k_status_t ret = k_status_ok;
	bool reesched = false;
	tcb_t *t = thread_get_current();
	ULIPE_ASSERT(t != NULL);


	if(s == NULL) {
		ret = k_status_invalid_param;
		goto cleanup;
	}

	if(port_from_isr()){
		/* take cannot be called from ISR */
		ret = k_status_illegal_from_isr;
		goto cleanup;
	}


	if(!s->created) {
		/* handle first time usage */
		k_work_list_init(&s->threads_pending);
		s->created = true;
	}


	ret = k_sched_lock();
	ULIPE_ASSERT(ret == k_status_ok);

	if(!s->cnt) {
		/*
		 * if no key available, we need to insert the waiting thread
		 * on semaphore pending list, when a key will become available
		 * the task will be woken as well
		 */
		ret = k_make_not_ready(t);
		ULIPE_ASSERT(ret == k_status_ok);

		t->thread_wait |= K_THR_PEND_SEMA;

		ret = k_pend_obj(t, &s->threads_pending);
		ULIPE_ASSERT(ret == k_status_ok);

		reesched = true;
	} else {

		/*
		 * with priority ceilling we give semaphore and also
		 * bring the task to the sys priority to improve its
		 * execution speed;
		 */
		semaphore_handle_celling(s, t);
	}


	if(!reesched){
		ret = k_sched_unlock();
		ULIPE_ASSERT(ret == k_status_ok);
		goto cleanup;
	}

	ret = k_sched_unlock();
	ULIPE_ASSERT(ret == k_status_ok);
	/*
	 * if current thread entered on pending state, we need to reesched the
	 * thread set and find a new thread to execute, otherwise, dispatch idle
	 */
	k_sched_and_swap();

	/*
	 * thread woken, immediately give a semaphore to it
	 */
	semaphore_handle_celling(s, t);



cleanup:
	return(ret);

}

k_status_t semaphore_give(ksema_t *s, uint32_t count)
{
	k_status_t ret = k_status_ok;
	tcb_t *t = NULL;

	if(s == NULL) {
		ret = k_status_invalid_param;
		goto cleanup;
	}

	if(!count) {
		ret = k_status_invalid_param;
		goto cleanup;
	}

	if(!s->created) {
		/* handle first time usage */
		k_work_list_init(&s->threads_pending);
		s->created = true;
	}

	ret = k_sched_lock();
	ULIPE_ASSERT(ret == k_status_ok);

	s->cnt+= count;
	if(s->cnt > s->limit)
		s->cnt = s->limit;

	semahore_restore_prio(s,thread_get_current());

	/*
	 * once a semaphore was updated its keys
	 * we need to verify if a new highprio task is available to
	 * run
	 */
	t = k_unpend_obj(&s->threads_pending);
	if(t == NULL) {
		/* no tasks pendings, just get out here */
		ret = k_sched_unlock();
		ULIPE_ASSERT(ret == k_status_ok);
		goto cleanup;
	}

	t->thread_wait &= ~(K_THR_PEND_SEMA);

	ret = k_make_ready(t);
	ULIPE_ASSERT(ret == k_status_ok);

	ret = k_sched_unlock();
	ULIPE_ASSERT(ret == k_status_ok);


	k_sched_and_swap();

cleanup:
	return(ret);

}
