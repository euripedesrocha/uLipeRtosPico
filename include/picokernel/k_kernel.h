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
#define K_PRIORITY_LEVELS	 	 4
#define K_SYS_THREAD_PRIO   	-3
#define K_IDLE_THREAD_PRIO		 0

/* define kernel working lists */
typedef struct {
	uint8_t bitmap;
	k_list_t list_head[ (2 * K_PRIORITY_LEVELS) - 1];
}k_work_list_t;


/**
 *  @fn k_pend_obj()
 *  @brief pend a task for a particular objects inserting in its list
 *  @param
 *  @return
 */
k_status_t k_pend_obj(tcb_t *thr, k_work_list_t *obj_list);

/**
 *  @fn k_unpend_obj()
 *  @brief remove a task from a pendable object list
 *  @param
 *  @return
 */
tcb_t *k_unpend_obj(k_work_list_t *obj_list);

/**
 *  @fn k_make_ready()
 *  @brief makes a particular tcb ready inserting into ready list task
 *  @param
 *  @return
 */
k_status_t k_make_ready(tcb_t *thr);

/**
 *  @fn k_make_not_ready()
 *  @brief remove a particular tcb from ready list
 *  @param
 *  @return
 */
k_status_t k_make_not_ready(tcb_t *thr);


/**
 *  @fn k_yield()
 *  @brief send the specified tcb to the back of its current FIFO
 *  @param
 *  @return
 */
bool k_yield(tcb_t *t);


/**
 *  @fn k_sched_and_swap()
 *  @brief schedule the task set and swap to the next ready to run
 *  @param
 *  @return
 */
k_status_t k_sched_and_swap(void);


/**
 *  @fn k_bit_set()
 *  @brief bit set primitive
 *  @param
 *  @return
 */
static inline archtype_t k_bit_set(archtype_t reg, uint8_t bit)
{
	return(reg |= (1 << bit));
}

/**
 *  @fn k_bit_clr()
 *  @brief bit clear primitive
 *  @param
 *  @return
 */
static inline archtype_t k_bit_clr(archtype_t reg, uint8_t bit)
{
	return((reg &= ~(1 << bit)));
}

/**
 *  @fn k_work_list_init()
 *  @brief inits a system list to default state
 *  @param
 *  @return
 */
void k_work_list_init(k_work_list_t *l);


/**
 *  @fn kernel_init()
 *  @brief initializes kernel to initial state
 *  @param
 *  @return
 */
k_status_t kernel_init(void);


/**
 *  @fn kernel_start()
 *  @brief perform first context switch and starts the kernel
 *  @param
 *  @return
 */
k_status_t kernel_start(void);

/**
 *  @fn kernel_irq_in()
 *  @brief creates a rtos irq safe area
 *  @param
 *  @return
 */
void kernel_irq_in(void);

/**
 *  @fn kernel_irq_out()
 *  @brief destroys a rtos irq safe area
 *  @param
 *  @return
 */
void kernel_irq_out(void);

/**
 *  @fn kernel_exception()
 *  @brief used to trap the kernel when a fault occurs
 *  @param
 *  @return
 */
//void kernel_exception(void);

#endif
