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

#include "ulipe_rtos_pico.h"

/**
 *  @fn port_irq_lock()
 *  @brief arch specific irq locking, disables the IRQs
 *
 *  @param	none
 *
 *  @return key with the current interrupt state
 */
extern archtype_t port_irq_lock(void);

/**
 *  @fn port_irq_unlock()
 *  @brief arch specific irq unlocking, enables the IRQs
 *
 *  @param pattern - key with the saved IRQs states
 *
 *  @return none
 */
extern void port_irq_unlock(archtype_t pattern);

/**
 *  @fn port_from_isr()
 *  @brief check if current execution is ocurring from a isr
 *
 *  @param none
 *
 *  @return true if current execution context is a ISR
 */
extern bool port_from_isr(void);


/**
 *  @fn port_create_stack_frame()
 *  @brief architecture specific stack frame creation for a thread
 *
 *  @param stack - pointer to area used as stack of thread
 *  @param thr_func - entry point of thread being created
 *  @param cookie - pointer to a custom argument passed to thread during creation
 *
 *  @return pointer to the modified stack filled with archtecture expected values
 */
archtype_t *port_create_stack_frame(archtype_t *stack, thread_t thr_func, void *cookie);


/**
 *  @fn port_swap_req()
 *  @brief Pend a syscall to switch the context
 *
 *  @param none
 *
 *  @return	none
 */
extern void port_swap_req(void);


/**
 *  @fn port_start_kernel()
 *  @brief platform specific kernel starting, triggers the scheduler
 *
 *  @param	none
 *
 *  @return none
 *
 *  NOTE: This function should never returns!
 */
extern void port_start_kernel(void);


/**
 *  @fn port_init_machine
 *  @brief inits machine specifics (priority levels, handlers, etc)
 *
 *  @param	none
 *
 *  @return none
 */
extern void port_init_machine(void);


/**
 *  @fn port_bit_fs_scan()
 *  @brief perform bit forward scan
 *
 *  @param reg - 32bit maximum wide word to be scanned
 *
 *  @return value between 0 - 31
 */
extern uint8_t port_bit_fs_scan(archtype_t reg);

/**
 *  @fn port_bit_ls_scan
 *  @brief performs a reverse bit scan
 *
 *  @param reg - 32bit maximum wide word to be scanned
 *
 *  @return value between 0 - 31
 */
extern uint8_t port_bit_ls_scan(archtype_t reg);

/**
 *  @fn port_set_break
 *  @brief set breakpoint, used in assertion mechanisms
 *
 *  @param none
 *
 *  @return none
 */
#if K_DEBUG > 0
extern void port_set_break(void);
#endif

/************* Specific low power port **********************************/
#if (K_ENABLE_TICKLESS_IDLE > 0)

/**
 *  @fn port_set_break
 *  @brief architecture specific low power engine handler, called by idle task
 *
 *  @param info - structure which contains the kernel states about timing and tasks
 *
 *  @return none
 */
extern void port_low_power_engine(k_wakeup_info_t *info);
#endif


/************* USER IMPLEMENTED HOOKS ***********************************/

/**
 *  @fn port_start_timer()
 *  @brief inits machine specific timer. if zero received, stop ticker
 *  @param
 *  @return
 */
extern void port_start_timer(uint32_t reload_val);


/**
 *  @fn port_timer_load_append()
 *  @brief adds a amount of period on timer
 *  @param
 *  @return
 */
extern void port_timer_load_append(uint32_t append_val);


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
 *  @fn timer_match_handler()
 *  @brief  output compare timer handler used by tickless engine
 *  @param
 *  @return
 */
extern void timer_match_handler(void);


/**
 *  @fn timer_ovf_handler()
 *  @brief timer overflow handling 
 *  @param
 *  @return
 */
extern void timer_ovf_handler(void);

#if(K_ENABLE_TICKLESS_IDLE > 0)
/**
 *  @fn port_start_ticker()
 *  @brief inits machine specific timer. if zero received, stop ticker
 *  @param
 *  @return
 */
extern void port_start_ticker(uint32_t reload_val);


/**
 *  @fn port_halt_ticker()
 *  @brief stops timer counting amd return its current value
 *  @param
 *  @return
 */
extern uint32_t port_halt_ticker(void);



#endif





#endif
