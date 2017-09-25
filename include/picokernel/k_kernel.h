/**
 * 							ULIPE RTOS PICO
 *
 *  @file k_kernel.h
 *
 *  @brief multithreading core header file
 *
 *
 */

#ifndef __K_KERNEL_H
#define __K_KERNEL_H


/* kernel always will have this priority level */
#define K_PRIORITY_LEVELS	 	 32
#define K_SYS_THREAD_PRIO   	 31
#define K_IDLE_THREAD_PRIO		 0xFF


/* define kernel system/pendable list */
typedef struct k_work_list{
	/* contains the bits sets corresponding to populated lists*/
	uint32_t bitmap;

	/* simple multilevel list which receives tcb in fifo form */
	k_list_t list_head[K_PRIORITY_LEVELS];

}k_work_list_t;


/**
 *  @fn k_pend_obj()
 *  @brief pend a task for a particular objects inserting in its list
 *
 *  @param thr - thread to pend
 *  @param obj_list - kernel object wait list
 *
 *  @return k_status_ok or error
 */
k_status_t k_pend_obj(tcb_t *thr, k_work_list_t *obj_list);

/**
 *  @fn k_unpend_obj()
 *  @brief remove the highest priority task from a pendable object list
 *
 *  @param obj_list - kernel object wait list which contains threads
 *
 *  @return tcb with highest priority task or NULL if is empty
 */
tcb_t *k_unpend_obj(k_work_list_t *obj_list);

/**
 *  @fn k_make_ready()
 *  @brief makes a particular tcb ready inserting into ready list task
 *
 *  @param thr - tcb containing the thread who wants to become ready
 *
 *  @return k_status_ok or error in case of invalid value
 */

k_status_t k_make_ready(tcb_t *thr);

/**
 *  @fn k_make_not_ready()
 *  @brief remove a particular tcb from ready list
 *
 *  @param thr - tcb containing the thread who wants to remove
 *
 *  @return k_status_ok or error in case of invalid value
 */
k_status_t k_make_not_ready(tcb_t *thr);


/**
 *  @fn k_yield()
 *  @brief send the specified tcb to the back of its current FIFO
 *
 *  @param t - tcb containing the thread to perform yielding
 *
 *  @return true if reeschedule is needed
 *
 *  NOTE: this task does not perform reescheduling automatically!!
 */
bool k_yield(tcb_t *t);


/**
 *  @fn k_sched_and_swap()
 *  @brief schedule the task set and swap to the next ready to run
 *
 *  @param none
 *
 *  @return k_status_ok if scheduler is perfomred or error if scheduler is locked
 *
 */
k_status_t k_sched_and_swap(void);


/**
 *  @fn k_bit_set()
 *  @brief bit set primitive
 *
 *  @param reg - word to perform bit set
 *  @param bit - bit index to set
 *
 *  @return word with newly set bit
 */
static inline archtype_t k_bit_set(archtype_t reg, uint8_t bit)
{
	return(reg |= (1 << bit));
}



/**
 *  @fn k_bit_clr()
 *  @brief bit clear primitive
 *
 *  @param reg - word to perform bit clear
 *  @param bit - bit index to clear
 *
 *  @return word with the newly cleared bit
 */
static inline archtype_t k_bit_clr(archtype_t reg, uint8_t bit)
{
	return((reg &= ~(1 << bit)));
}


/**
 *  @fn k_work_list_init()
 *  @brief inits a system/pendable  list to default state
 *
 *  @param l - pendable/system list to be initialized
 *
 *  @return	none
 */
void k_work_list_init(k_work_list_t *l);


/**
 *  @fn kernel_init()
 *  @brief initializes kernel to initial state, needs to be called before any other kernel api
 *
 *  @param none
 *
 *  @return k_status_ok on succesful kernel initialization
 */
k_status_t kernel_init(void);


/**
 *  @fn kernel_start()
 *  @brief starts the scheduler and gives CPU control to kernel
 *
 *  @param	none
 *
 *  @return this routine should not return, but if does it returns a error code
 */
k_status_t kernel_start(void);


/**
 *  @fn kernel_irq_in()
 *  @brief creates a rtos irq safe area needs to be called at every ISR entry before use any kernal API
 *
 *  @param none
 *
 *  @return none
 */
void kernel_irq_in(void);


/**
 *  @fn kernel_irq_out()
 *  @brief destroys a rtos irq safe area and request a scheduling, needs to be called after all apis of kernel was invoked in ISR
 *
 *  @param	none
 *
 *  @return	none
 */
void kernel_irq_out(void);


#endif
