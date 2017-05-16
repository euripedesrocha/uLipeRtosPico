/**
 * 							ULIPE RTOS PICO
 *
 *  @file k_port.h
 *
 *  @brief architecture portable header file
 *
 *
 */
#ifndef __K_PORT_H
#define __K_PORT_H


/**
 *  @fn port_irq_lock()
 *  @brief arch specific irq locking
 *  @param
 *  @return
 */
extern archtype_t port_irq_lock(void);

/**
 *  @fn port_irq_unlock()
 *  @brief arch specific irq unlocking
 *  @param
 *  @return
 */
extern void port_irq_unlock(archtype_t pattern);

/**
 *  @fn port_from_isr()
 *  @brief check if current execution is ocurring from isr
 *  @param
 *  @return
 */
extern bool port_from_isr(void);

/**
 *  @fn port_create_stack_frame()
 *  @brief architecture specific stack frame creation for a thread
 *  @param
 *  @return
 */
archtype_t *port_create_stack_frame(archtype_t *stack, thread_t thr_func, void *cookie);


/**
 *  @fn port_swap_req()
 *  @brief perfoms a swap function calling
 *  @param
 *  @return
 */
extern void port_swap_req(void);


/**
 *  @fn port_start_kernel()
 *  @brief platform specific kernel start
 *  @param
 *  @return
 */
extern void port_start_kernel(void);

/**
 *  @fn port_init_machine
 *  @brief inits machine specifis (priority levels, handlers, etc)
 *  @param
 *  @return
 */
extern void port_init_machine(void);


/**
 *  @fn port_start_timer()
 *  @brief inits machine specific timer. if zero received, stop ticker
 *  @param
 *  @return
 */
extern void port_start_timer(archtype_t reload_val);


/**
 *  @fn port_timer_load_append()
 *  @brief adds a amount of period on timer
 *  @param
 *  @return
 */
extern void port_timer_load_append(archtype_t append_val);


/**
 *  @fn port_timer_halt()
 *  @brief stops timer counting amd return its current value
 *  @param
 *  @return
 */
extern uint32_t port_timer_halt(void);


/**
 *  @fn port_timer_resume()
 *  @brief resumes timer counting
 *  @param
 *  @return
 */
extern void port_timer_resume(void);



/**
 *  @fn port_bit_fs_scan()
 *  @brief perform bit forward scan
 *  @param
 *  @return
 */
extern uint8_t port_bit_fs_scan(archtype_t reg);

/**
 *  @fn port_bit_ls_scan
 *  @brief performs a reverse bit scan
 *  @param
 *  @return
 */
extern uint8_t port_bit_ls_scan(archtype_t reg);

/**
 *  @fn port_set_break
 *  @brief set breakpoint
 *  @param
 *  @return
 */
#if K_DEBUG > 0
extern void port_set_break(void);
#endif


#endif
