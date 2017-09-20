/**
 * 							ULIPE RTOS PICO
 *
 *  @file k_wqueue.c
 *
 *  @brief workqueues job deferrable server
 *
 *
 */


#include "ulipe_rtos_pico.h"

#if(K_ENABLE_WORKQUEUES > 0)

/* signal to notify deferrable server to wake up */
#define WQ_DEFER_JOB_FLAG	0x01

/**
 *  @fn wqueue_defer_handler()
 *  @brief dispatches the next head job to the handler in this wq
 *  @param
 *  @return
 */
static void wqueue_defer_handler(void *wq_ctl)
{
	wqueue_t * wq = (wqueue_t *)wq_ctl;
	k_status_t err = k_status_ok;
	uint32_t signals = 0;
	wqueue_job_t *job = NULL;

	for(;;) {

		if(!signals) {
			signals = thread_wait_signals(wq->thr, WQ_DEFER_JOB_FLAG, k_wait_match_pattern, &err);
			if(err != k_status_ok) continue;
		}

		/* signaled thread, fifo has a entry, obtain it and submit job
		 * to user custom process
		 */
		archtype_t key = port_irq_lock();


		k_list_t *head = NULL;
		head = sys_dlist_peek_head(&wq->fifo);
		if(!head) {
			thread_clr_signals(wq->thr, signals);
			signals &= ~WQ_DEFER_JOB_FLAG;
			port_irq_unlock(key);

		} else {
			sys_dlist_remove(head);
			job = CONTAINER_OF(head, struct wqueue_job, node);
			port_irq_unlock(key);

			/* job never to be invalid if reached here */
			ulipe_assert(job != NULL);

			/* execute handler */
			if(job->handler)
				job->handler(job);

			/* avoid the deferrable server to hang if queue
			 * becames populated too frequently
			 */
			thread_yield();

		}
	}
}


/** public functions */
k_status_t wqueue_init(wqueue_t *wq)
{
	k_status_t ret = k_status_ok;

	if(wq == NULL) {
		ret = k_status_invalid_param;
		goto cleanup;
	}



	/* if the current workqueue already exists,
	 * ignore this step
	 */
	if(wq->thr->created) {
		ret = k_wqueue_already_exists;
		goto cleanup;
	}

	archtype_t key = port_irq_lock();

	/* inits the wq fifo */
	sys_dlist_init(&wq->fifo);

	port_irq_unlock(key);

	/* starts the wqueue deferrable thread*/
	ret = thread_create(wqueue_defer_handler, wq, wq->thr);
	ulipe_assert(ret == k_status_ok);


cleanup:
	return(ret);
}



k_status_t wqueue_submit(wqueue_t *wq, wqueue_job_t *work)
{
	k_status_t ret = k_status_ok;

	if(wq == NULL) {
		ret = k_status_invalid_param;
		goto cleanup;
	}

	if(work == NULL) {
		ret = k_status_invalid_param;
		goto cleanup;
	}

	archtype_t key = port_irq_lock();
	sys_dlist_append(&wq->fifo, &work->node);
	port_irq_unlock(key);

	thread_set_signals(wq->thr, WQ_DEFER_JOB_FLAG);

cleanup:
	return(ret);
}


#endif
