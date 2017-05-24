/**
 * 							ULIPE RTOS PICO
 *
 *  @file k_message.c
 *
 *  @brief basic messaging usage file
 *
 */

#include "ulipe_rtos_pico.h"

#if (K_ENABLE_MESSAGING > 0)

/** private functions */


/**
 *  @fn message_handle_pend()
 *  @brief check kmsgt internal thread list for a insertion waiting task
 *  @param
 *  @return
 */
static k_status_t message_handle_pend(kmsg_t *m,bool insert, msg_opt_t opt, archtype_t key)
{
	k_status_t ret = false;
	tcb_t *thr = thread_get_current();
	ULIPE_ASSERT(thr != NULL);



	switch(opt) {
		case k_msg_accept:
			/* simplest case, accept there is no space left
			 * on message queue and reports error
			 */
			if(insert)
				ret = k_queue_full;
			else
				ret = k_queue_empty;


			break;

		case k_msg_block:
			/* thread will wait until one slot become free */
			k_make_not_ready(thr);
			thr->thread_wait |= K_THR_PEND_MSG;

			if(insert)
				k_pend_obj(thr, &m->wr_threads_pending);
			else
				k_pend_obj(thr, &m->rd_threads_pending);


			port_irq_unlock(key);
			ret = k_sched_and_swap();
			ULIPE_ASSERT(ret == k_status_ok);

			/* thread woken, lock acess to queue again */
			ret = k_status_ok;
			key = port_irq_lock();
			break;

		default:
			ret = k_status_invalid_param;
			break;
	}

	return(ret);
}

/** Public functions */


k_status_t message_insert(kmsg_t *m, void *data, uint32_t size, msg_opt_t opt)
{
	k_status_t ret = k_status_ok;
	archtype_t key;

	if(m == NULL) {
		ret = k_status_invalid_param;
		goto cleanup;
	}

	if(data == NULL) {
		ret = k_status_invalid_param;
		goto cleanup;
	}

	if(size > m->slot_size) {
		ret = k_status_invalid_param;
		goto cleanup;
	}

	if(!size) {
		ret = k_status_invalid_param;
		goto cleanup;
	}

	key = port_irq_lock();

	if(!m->created) {
		m->created = true;
		k_work_list_init(&m->wr_threads_pending);
		k_work_list_init(&m->rd_threads_pending);
	}

	if(m->items >= m->slots_number) {
		/* message queue is actually full,
		 * so check the wait options passed by user
		 */
		ret = message_handle_pend(m,true, opt, key);

		if(ret != k_status_ok) {
			port_irq_unlock(key);
			goto cleanup;
		}
	}


	/* regular use case, insert a new frame on message
	 * queue and wake up a possible thread set
	 */
	archtype_t *ptr = (archtype_t *)&m->data[m->wr_ptr * (m->slot_size)];
	ULIPE_ASSERT(ptr != NULL);
	*ptr++ = size;

	if(size == sizeof(archtype_t)) {
		/* special case to avoid copy and creates
		 * a by reference queue support
		 */
		*ptr = *((archtype_t*)data);

	} else {

		/* just a ordinary copy */
		memcpy(&m->data[(m->wr_ptr * (m->slot_size)) + sizeof(archtype_t)], data, size);
	}

	m->wr_ptr = (m->wr_ptr + 1) % m->slots_number;
	m->items++;



	tcb_t *thr = k_unpend_obj(&m->rd_threads_pending);
	if(thr == NULL) {
		/* no need to reeschedule task list */
		port_irq_unlock(key);
		goto cleanup;
	}

	thr->thread_wait &= ~(K_THR_PEND_MSG);
	ret = k_make_ready(thr);
	ULIPE_ASSERT(ret == k_status_ok);
	port_irq_unlock(key);


	/* reeschedule task set */
	ret = k_sched_and_swap();
	ULIPE_ASSERT(ret == k_status_ok);

cleanup:
	return(ret);
}



k_status_t message_remove(kmsg_t *msg, void *data, uint32_t *size, bool peek, msg_opt_t opt)
{
	k_status_t ret = k_status_ok;
	archtype_t key;

	if(msg== NULL) {
		ret = k_status_invalid_param;
		goto cleanup;
	}

	if(data == NULL) {
		ret = k_status_invalid_param;
		goto cleanup;
	}

	if(size == NULL) {
		ret = k_status_invalid_param;
		goto cleanup;
	}

	key = port_irq_lock();

	if(!msg->created) {
		msg->created = true;
		k_work_list_init(&msg->wr_threads_pending);
		k_work_list_init(&msg->rd_threads_pending);
	}



	if(msg->items == 0) {
		/* message queue is actually empty,
		 * so check the wait options passed by user
		 */
		ret = message_handle_pend(msg,false, opt,key);

		if(ret != k_status_ok) {
			port_irq_unlock(key);
			goto cleanup;
		}

	}

	archtype_t *ptr = (archtype_t *)&msg->data[msg->rd_ptr * (msg->slot_size )];
	ULIPE_ASSERT(ptr != NULL);
	archtype_t data_size = *ptr++;

	/*
	 * we have at least one slot pending for remotion, lets pick
	 * it
	 */
	if(data_size == sizeof(archtype_t)) {
		/* special case to avoid copy and creates
		 * a by reference queue support
		 */
		*((archtype_t*)data) = *ptr;
	} else {

		/* just a ordinary copy */
		memcpy(data, &msg->data[(msg->rd_ptr * msg->slot_size) + sizeof(archtype_t)  ], data_size);
	}
	*size = data_size;


	/*
	 * if a peek was requested remove the entry, otherwise, nothing changes
	 * just exit
	 */
	if(peek) {
		port_irq_unlock(key);
		goto cleanup;
	}

	msg->rd_ptr = (msg->rd_ptr + 1) % msg->slots_number;
	msg->items--;

	tcb_t *thr = k_unpend_obj(&msg->wr_threads_pending);
	if(thr == NULL) {
		port_irq_unlock(key);
		goto cleanup;
	}


	ret = k_make_ready(thr);
	ULIPE_ASSERT(ret == k_status_ok);
	port_irq_unlock(key);

	ret = k_sched_and_swap();
	ULIPE_ASSERT(ret == k_status_ok);
cleanup:
	return(ret);
}


#endif
