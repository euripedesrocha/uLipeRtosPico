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
THREAD_CONTROL_BLOCK_DECLARE(idle_thread, 64, 0);



static k_work_list_t k_rdy_list;
static k_list_t k_timed_list;

static bool k_configured;
static archtype_t irq_counter;
uint32_t irq_lock_nest;

/** public variables **/
bool k_running = false;
tcb_t *k_current_task;
tcb_t *k_high_prio_task;
archtype_t irq_nesting = 0;

static k_wakeup_info_t wu_info;


#if((K_ENABLE_TICKER > 0) || (K_ENABLE_TIMERS > 0))
extern tcb_t timer_tcb;
extern void timer_dispatcher(void *args);
#endif

/** private functions **/



/**
 *  @fn k_idle_thread()
 *  @brief idle thread to be executed when no other thread are ready to run
 *  @param
 *  @return
 */
static void k_idle_thread(void *kernel_info)
{
	k_wakeup_info_t *info = (k_wakeup_info_t *)kernel_info;
	ulipe_assert(info != NULL);


	for(;;) {
#if (K_ENABLE_TICKLESS_IDLE > 0)
		/* kernel can sleep ? */
		if((info->next_thread_wake == NULL) && (info->next_timer == NULL)) {
			/* simplest case, we need only to enter in sleep for user defined time
			 * the tasks sleep for defined time is future implementation
			 */



		}

#else
		(void)info;
#endif
	}
}

/**
 *  @fn k_sched()
 *  @brief gets the highest priority and most recet task to run
 *  @param
 *  @return
 */
static tcb_t *k_sched(k_work_list_t *l)
{
	tcb_t *ret = &idle_thread;
	k_list_t *head;

#if K_DEBUG > 0
	if(k_running) {
		k_current_task->stk_usage = (k_current_task->stack_top - k_current_task->stack_base);
		/* stack monitor used during debug */
		ULIPE_ASSERT( k_current_task->stk_usage * sizeof(archtype_t) <=  k_current_task->stack_size * sizeof(archtype_t));
	}
#endif



	ULIPE_ASSERT(l != NULL);

	/* no tasks ready, just hint to kernel to load the idle task */
	if(!l->bitmap)
		goto cleanup;

	/*
	 * The scheduling alghoritm uses a classical multilevel
	 * fifo ensuring a O(1) time complexity, the way to obtain the highest priority and first in
	 * task is performing a bit search on priority bitmap, since the
	 * priority levels is 4 + some sys priority only a simple
	 * lead zero table perform this action, with the prio obtained we
	 * get the head of the list indexed to this prio and
	 * finally access the tcb
	 */
	uint8_t prio = (K_PRIORITY_LEVELS - 1) - port_bit_fs_scan(l->bitmap);
	
	head = sys_dlist_peek_head(&l->list_head[prio]);
	if(head != NULL)
		ret = CONTAINER_OF(head, tcb_t, thr_link);

cleanup:
	return(ret);
}



/** public functions **/
k_status_t k_pend_obj(tcb_t *thr, k_work_list_t *obj_list)
{
	k_status_t err = k_status_ok;
	ULIPE_ASSERT(thr != NULL);
	ULIPE_ASSERT(obj_list != NULL);

#if K_DEBUG > 0
	if(k_running) {
		k_current_task->stk_usage = (k_current_task->stack_top - k_current_task->stack_base);
		/* stack monitor used during debug */
		ULIPE_ASSERT( k_current_task->stk_usage * sizeof(archtype_t) <=  k_current_task->stack_size * sizeof(archtype_t));
	}
#endif



	/*
	 * The insertion on obj_list consider that tasks are using priority ceilling
	 * or system threads, which priorities are < 0 , so we need to handle
	 * both cases, for negative priority we remove the signal and use its upper
	 * 3 bits to insert in system level priorities list
	 */

	obj_list->bitmap |= (1 << thr->thread_prio);
	sys_dlist_append(&obj_list->list_head[thr->thread_prio], &thr->thr_link);


	return(err);

}

tcb_t * k_unpend_obj(k_work_list_t *obj_list)
{
	tcb_t *thr = k_status_ok;
	ULIPE_ASSERT(obj_list != NULL);

#if K_DEBUG > 0
	if(k_running) {
		k_current_task->stk_usage = (k_current_task->stack_top - k_current_task->stack_base);
		/* stack monitor used during debug */
		ULIPE_ASSERT( k_current_task->stk_usage * sizeof(archtype_t) <=  k_current_task->stack_size * sizeof(archtype_t));
	}
#endif


	/*
	 * unpend a obj is a little bit complex relative to a
	 * make not ready, before to remove a item from a work list
	 * we need to know which object is the desired, to keep the
	 * kernel execution policy, we execute the fifo-sched on
	 * work list to obtain which is the highest priority task
	 * waiting for a kernel object and remove it from list
	 */
	thr = k_sched(obj_list);
	if(thr == &idle_thread) {
		thr = NULL;
		goto cleanup;
	}

	sys_dlist_remove(&thr->thr_link);
	if(sys_dlist_is_empty(&obj_list->list_head[thr->thread_prio])){
		obj_list->bitmap &= ~(1 << thr->thread_prio);
	}

cleanup:
	return(thr);
}


k_status_t k_make_ready(tcb_t *thr)
{
	k_status_t err = k_status_ok;
	ULIPE_ASSERT(thr != NULL);

#if K_DEBUG > 0
	if(k_running) {
		k_current_task->stk_usage = (k_current_task->stack_top - k_current_task->stack_base);
		/* stack monitor used during debug */
		ULIPE_ASSERT( k_current_task->stk_usage * sizeof(archtype_t) <=  k_current_task->stack_size * sizeof(archtype_t));
	}
#endif

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
	k_rdy_list.bitmap |= (1 << thr->thread_prio);
	sys_dlist_append(&k_rdy_list.list_head[thr->thread_prio], &thr->thr_link);


cleanup:
	return(err);
}

k_status_t k_make_not_ready(tcb_t *thr)
{
	k_status_t err = k_status_ok;
	ULIPE_ASSERT(thr != NULL);

	/*
	 * for remotion we need care both priority cases as well,
	 * and before clear a priorty level bitmap, we need to make sure
	 * there is no further tasks which share the priority of thr
	 *
	 */

#if K_DEBUG > 0
	if(k_running) {
		k_current_task->stk_usage = (k_current_task->stack_top - k_current_task->stack_base);
		/* stack monitor used during debug */
		ULIPE_ASSERT( k_current_task->stk_usage * sizeof(archtype_t) <=  k_current_task->stack_size * sizeof(archtype_t));
	}
#endif



	sys_dlist_remove(&thr->thr_link);
	if(sys_dlist_is_empty(&k_rdy_list.list_head[thr->thread_prio])) {
		k_rdy_list.bitmap &= ~(1 << thr->thread_prio);
	}



	return(err);

}

bool k_yield(tcb_t *t)
{
	k_status_t err = k_status_ok;
	ULIPE_ASSERT(t != NULL);

#if K_DEBUG > 0
	if(k_running) {
		k_current_task->stk_usage = (k_current_task->stack_top - k_current_task->stack_base);
		/* stack monitor used during debug */
		ULIPE_ASSERT( k_current_task->stk_usage * sizeof(archtype_t) <=  k_current_task->stack_size * sizeof(archtype_t));
	}
#endif



	bool resched = false;


	err = k_make_not_ready(t);
	ULIPE_ASSERT(err == k_status_ok);


	err = k_make_ready(t);
	ULIPE_ASSERT(err == k_status_ok);

	/* check if anything changed durring send to back
	 * operation
	 */
	if(k_sched(&k_rdy_list) != t) {
		resched = true;
	}

	return(resched);
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
	if(k_high_prio_task == NULL) {
		/* no other tasks ready to run, puts the idle thread */
		k_high_prio_task = &idle_thread;
	}


#if K_DEBUG > 0
	if(k_running) {
		k_current_task->stk_usage = (k_current_task->stack_top - k_current_task->stack_base);
		/* stack monitor used during debug */
		ULIPE_ASSERT( k_current_task->stk_usage * sizeof(archtype_t) <=  k_current_task->stack_size * sizeof(archtype_t));
	}
#endif



	if(k_high_prio_task != k_current_task) {
		/* new high priority task, perform a swap request */
		port_swap_req();
	}

cleanup:
	return(ret);
}



void k_work_list_init(k_work_list_t *l)
{

#if K_DEBUG > 0
	if(k_running) {
		k_current_task->stk_usage = (k_current_task->stack_top - k_current_task->stack_base);
		/* stack monitor used during debug */
		ULIPE_ASSERT( k_current_task->stk_usage * sizeof(archtype_t) <=  k_current_task->stack_size * sizeof(archtype_t));
	}
#endif


	l->bitmap = 0;
	for(uint8_t i=0; i < K_PRIORITY_LEVELS ; i++)
		sys_dlist_init(&l->list_head[i]);
}


k_status_t kernel_init(void)
{
	irq_lock_nest = 0;

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

	extern tcb_t *next_task_wake;
	extern uint32_t tick_count;
	extern ktimer_t *actual_timer;

	wu_info.next_thread_wake = next_task_wake;
	wu_info.next_timer = actual_timer;
	wu_info.tick_cntr = &tick_count;

	/* creates the idle thread */
	k_status_t err = thread_create(&k_idle_thread,&wu_info, &idle_thread);
	ULIPE_ASSERT(err == k_status_ok);

#if(K_ENABLE_DYNAMIC_ALLOCATOR > 0)
	extern void k_heap_init(void);
	k_heap_init();
#endif

	k_make_not_ready(&idle_thread);
	idle_thread.thread_prio = K_IDLE_THREAD_PRIO;

#if(K_ENABLE_TICKER > 0)
	err = thread_create(&timer_dispatcher,NULL, &timer_tcb);
	ULIPE_ASSERT(err == k_status_ok);
#endif

	/* os configured and ready to be started */
	k_configured = true;

	return(k_status_ok);
}

k_status_t kernel_start(void)
{
	if(!k_configured)
		/* kernel must be configured before start */
		goto cleanup;

	/* init architecture stuff */
	port_init_machine();

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

#if K_DEBUG > 0
	if(k_running) {
		k_current_task->stk_usage = (k_current_task->stack_top - k_current_task->stack_base);
		/* stack monitor used during debug */
		ULIPE_ASSERT( k_current_task->stk_usage * sizeof(archtype_t) <=  k_current_task->stack_size * sizeof(archtype_t));
	}
#endif

	/* this function only can be called if os is executing  and from an isr*/
	if(!k_running)
		return;
#if(ARCH_TYPE_AVR_TINY > 0)
	(void)0;
#else 		
	if(!port_from_isr())
		return;
#endif

	if(irq_counter < (archtype_t)0xFFFFFFFF)
		irq_counter++;

	if(irq_lock_nest < (archtype_t)0xFFFFFFFF)
		irq_lock_nest++;

}

void kernel_irq_out(void)
{

#if K_DEBUG > 0
	if(k_running) {
		k_current_task->stk_usage = (k_current_task->stack_top - k_current_task->stack_base);
		/* stack monitor used during debug */
		ULIPE_ASSERT( k_current_task->stk_usage * sizeof(archtype_t) <=  k_current_task->stack_size * sizeof(archtype_t));
	}
#endif

	/* this function only can be called if os is executing  and from an isr*/
	if(!k_running)
		return;

	if(!port_from_isr())
		return;

	if(irq_lock_nest > (archtype_t)0)
		irq_lock_nest--;


	if(irq_counter > (archtype_t)0) {
		irq_counter--;
		/* all isrs attended, perform a system reeschedule */
		if(!irq_counter)
			k_sched_and_swap();
	}

}
