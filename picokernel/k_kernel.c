/**
 * 							ULIPE RTOS PICO
 *
 *  @file k_kernel.c
 *
 *  @brief multithreading core file
 *
 */

#include "ulipe_rtos_pico.h"

/**static variables **/
THREAD_CONTROL_BLOCK_DECLARE(idle_thread, 16, 0);

#if(K_ENABLE_TIMERS > 0)
THREAD_CONTROL_BLOCK_DECLARE(timer_thread, K_TIMER_DISPATCHER_STACK_SIZE, K_TIMER_DISPATCHER_PRIORITY);
#endif


static k_work_list_t k_rdy_list;
static k_list_t k_timed_list;

static bool k_configured;
static archtype_t irq_counter;
static const uint8_t k_clz_table[(1 << K_PRIORITY_LEVELS)] = {0x04, 0x03, 0x02, 0x02,
															  0x01, 0x01, 0x01, 0x01,
															  0x00, 0x00, 0x00, 0x00,
															  0x00, 0x00, 0x00, 0x00};


/** public variables **/
bool k_running = false;
tcb_t *k_current_task;
tcb_t *k_high_prio_task;


/** private functions **/


/**
 *  @fn k_idle_thread()
 *  @brief idle thread to be executed when no other thread are ready to run
 *  @param
 *  @return
 */
static void k_idle_thread(void *arg)
{
	/** todo: implement low power handling here */
	(void)arg;
	for(;;)
		thread_yield();
}

/**
 *  @fn k_sched()
 *  @brief gets the highest priority and most recet task to run
 *  @param
 *  @return
 */
static tcb_t *k_sched(k_work_list_t *l)
{
	tcb_t *ret;
	k_list_t *head;

	ULIPE_ASSERT(l != NULL);

	archtype_t key = port_irq_lock();

	/*
	 * The scheduling alghoritm uses a classical multilevel
	 * fifo ensuring a O(1) time complexity, the way to obtain the highest priority and first in
	 * task is performing a bit search on priority bitmap, since the
	 * priority levels is 4 + some sys priority only a simple
	 * lead zero table perform this action, with the prio obtained we
	 * get the head of the list indexed to this prio and
	 * finally access the tcb
	 */

	if(l->bitmap & (1 << (2 * (K_PRIORITY_LEVELS)- 1))) {
		uint8_t prio = (l->bitmap & 0x70) >> (K_PRIORITY_LEVELS - 1);
		prio = ((K_PRIORITY_LEVELS - 1) - k_clz_table[prio]);

		head = sys_dlist_peek_head(&l->list_head[prio + K_PRIORITY_LEVELS - 1]);
		ULIPE_ASSERT(head != NULL);
		ret = CONTAINER_OF(head, tcb_t, thr_link);

	} else {
		uint8_t prio = ((K_PRIORITY_LEVELS - 1) - k_clz_table[l->bitmap]);
		head = sys_dlist_peek_head(&l->list_head[prio]);
		ULIPE_ASSERT(head != NULL);
		ret = CONTAINER_OF(head, tcb_t, thr_link);

	}

	port_irq_unlock(key);

	return(ret);
}



/** public functions **/
k_status_t k_pend_obj(tcb_t *thr, k_work_list_t *obj_list)
{
	k_status_t err = k_status_ok;
	ULIPE_ASSERT(thr != NULL);
	ULIPE_ASSERT(obj_list != NULL);


	archtype_t key = port_irq_lock();

	/*
	 * The insertion on obj_list consider that tasks are using priority ceilling
	 * or system threads, which priorities are < 0 , so we need to handle
	 * both cases, for negative priority we remove the signal and use its upper
	 * 3 bits to insert in system level priorities list
	 */

	if(thr->thread_prio < 0) {
		uint8_t upper_prio = (((uint8_t)thr->thread_prio * -1 ) + K_PRIORITY_LEVELS - 1);
		obj_list->bitmap |= (1 << ((2 * K_PRIORITY_LEVELS)-1))+(1 << (upper_prio));
		sys_dlist_append(&obj_list->list_head[upper_prio], &thr->thr_link);

	} else {
		obj_list->bitmap |= (1 << thr->thread_prio);
		sys_dlist_append(&obj_list->list_head[thr->thread_prio], &thr->thr_link);
	}

	port_irq_unlock(key);
	return(err);

}

tcb_t * k_unpend_obj(k_work_list_t *obj_list)
{
	tcb_t *thr = k_status_ok;
	ULIPE_ASSERT(obj_list != NULL);


	/*
	 * unpend a obj is a little bit complex relative to a
	 * make not ready, before to remove a item from a work list
	 * we need to know which object is the desired, to keep the
	 * kernel execution policy, we execute the fifo-sched on
	 * work list to obtain which is the highest priority task
	 * waiting for a kernel object and remove it from list
	 */


	thr = k_sched(obj_list);
	if(thr == NULL)
		goto cleanup;

	archtype_t key = port_irq_lock();

	if(thr->thread_prio < 0) {
		uint8_t upper_prio = (((uint8_t)thr->thread_prio * -1 ) + K_PRIORITY_LEVELS - 1);
		sys_dlist_remove(&thr->thr_link);

		if(sys_dlist_is_empty(&obj_list->list_head[upper_prio])) {
			obj_list->bitmap &= ~(1 << upper_prio);
			if(!(obj_list->bitmap & 0x70))
				obj_list->bitmap &= ~(1 << ((2 * K_PRIORITY_LEVELS)-1));
		}

	} else {
		sys_dlist_remove(&thr->thr_link);
		if(sys_dlist_is_empty(&obj_list->list_head[thr->thread_prio])){
			obj_list->bitmap &= ~(1 << thr->thread_prio);
		}
	}

	port_irq_unlock(key);


cleanup:
	return(thr);
}

k_status_t k_make_ready(tcb_t *thr)
{
	k_status_t err = k_status_ok;
	ULIPE_ASSERT(thr != NULL);

	archtype_t key = port_irq_lock();

	/* to make ready evaluate if task does not waits for
	 * any more objects
	 */
	if((thr->thread_wait != 0))
		goto cleanup;

	/*
	 * The insertion on ready list consider that tasks are using priority ceilling
	 * or system threads, which priorities are < 0 , so we need to handle
	 * both cases, for negative priority we remove the signal and use its upper
	 * 3 bits to insert in system level priorities list
	 */

	if(thr->thread_prio < 0) {
		uint8_t upper_prio = (((uint8_t)thr->thread_prio * -1 ) + K_PRIORITY_LEVELS - 1);
		k_rdy_list.bitmap |= (1 << ((2 * K_PRIORITY_LEVELS)-1))+(1 << upper_prio);
		sys_dlist_append(&k_rdy_list.list_head[upper_prio], &thr->thr_link);

	} else {
		k_rdy_list.bitmap |= (1 << thr->thread_prio);
		sys_dlist_append(&k_rdy_list.list_head[thr->thread_prio], &thr->thr_link);
	}


cleanup:
	port_irq_unlock(key);
	return(err);
}

k_status_t k_make_not_ready(tcb_t *thr)
{
	k_status_t err = k_status_ok;
	ULIPE_ASSERT(thr != NULL);

	archtype_t key = port_irq_lock();

	/*
	 * for remotion we need care both priority cases as well,
	 * and before clear a priorty level bitmap, we need to make sure
	 * there is no further tasks which share the priority of thr
	 *
	 */


	if(thr->thread_prio < 0) {
		uint8_t upper_prio = (((uint8_t)thr->thread_prio * -1 ) + K_PRIORITY_LEVELS -1 );
		sys_dlist_remove(&thr->thr_link);

		if(sys_dlist_is_empty(&k_rdy_list.list_head[upper_prio])){
			k_rdy_list.bitmap &= ~(1 <<upper_prio);
			if(!(k_rdy_list.bitmap & 0x70)) {
				k_rdy_list.bitmap &= ~(1 << ((2 * K_PRIORITY_LEVELS)-1));
			}
		}

	} else {
		sys_dlist_remove(&thr->thr_link);
		if(sys_dlist_is_empty(&k_rdy_list.list_head[thr->thread_prio])) {
			k_rdy_list.bitmap &= ~(1 << thr->thread_prio);
		}


	}

	port_irq_unlock(key);
	return(err);

}

k_status_t k_sched_and_swap(void)
{
	k_status_t ret = k_status_ok;

	if(!k_running){
		ret = k_status_error;
		goto cleanup;
	}

	/*
	 * the sched and swap has the possibility to perform a
	 * context switch, so we need ensure the if in a ISR
	 * that all ISRs was processed and scheduling is the
	 * last ISR to process
	 */
	if(irq_counter > 0){
		ret = k_status_sched_locked;
		goto cleanup;
	}

	k_high_prio_task = k_sched(&k_rdy_list);

	ULIPE_ASSERT(k_high_prio_task != NULL);

	/* stack monitor used during debug */
	ULIPE_ASSERT((k_high_prio_task->stack_top - k_high_prio_task->stack_base) <=  k_high_prio_task->stack_size);

	if(k_high_prio_task != k_current_task) {
		/* new high priority task, perform a swap request */
		port_swap_req();
	}

cleanup:
	return(ret);
}



void k_work_list_init(k_work_list_t *l)
{
	l->bitmap = 0;
	for(uint8_t i=0; i < ((2 * K_PRIORITY_LEVELS) - 1); i++)
		sys_dlist_init(&l->list_head[i]);
}


k_status_t kernel_init(void)
{
	archtype_t key = port_irq_lock();

	/* no priority ready */
	k_work_list_init(&k_rdy_list);

	/* init timed list */
	sys_dlist_init(&k_timed_list);

	/* irq counter zeroed, default state */
	irq_counter = 0;

	/* current and hpt holding no task */
	k_current_task = NULL;
	k_high_prio_task = NULL;

	port_irq_unlock(key);

	/* creates the idle thread */
	k_status_t err = thread_create(&k_idle_thread,NULL, &idle_thread);
	ULIPE_ASSERT(err == k_status_ok);

	/* os configured and ready to be started */
	k_configured = true;

	return(k_status_ok);
}

k_status_t kernel_start(void)
{
	if(!k_configured)
		/* kernel must be configured before start */
		goto cleanup;

	archtype_t key = port_irq_lock();

	/* init architecture stuff */
	port_init_machine();
	port_irq_unlock(key);

	/* gets the first task to run */
	k_high_prio_task = k_sched(&k_rdy_list);

	ULIPE_ASSERT(k_high_prio_task != NULL);

	/* start kernel (this function will never return) */
	port_start_kernel();

cleanup:
	return(k_status_error);
}

void kernel_irq_in(void)
{
	/* this function only can be called if os is executing  and from an isr*/
	if(!k_running)
		goto cleanup;
	if(!port_from_isr())
		goto cleanup;

	if(irq_counter < (archtype_t)0xFFFFFFFF)
		irq_counter++;
cleanup:
	return;
}

void kernel_irq_out(void)
{
	/* this function only can be called if os is executing  and from an isr*/
	if(!k_running)
		goto cleanup;
	if(!port_from_isr())
		goto cleanup;

	if(irq_counter > (archtype_t)0)
		irq_counter--;

	/* all isrs attended, perform a system reeschedule */
	if(!irq_counter)
		k_sched_and_swap();
cleanup:
	return;
}
