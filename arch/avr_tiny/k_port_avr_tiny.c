/**
 * 							ULIPE RTOS PICO
 *
 *  @file k_port_avr_tiny.c
 *
 *  @brief specific avr tiny port file
 *
 *
 */

#include "ulipe_rtos_pico.h"
#include "include/arch/k_port_avr_tiny_defs.h"

#if(ARCH_TYPE_AVR_TINY > 0)



archtype_t *port_create_stack_frame(archtype_t *stack, thread_t thr_func, void *cookie)
{
    avr_tiny_xcpt_contents_t *ptr = ((avr_tiny_xcpt_contents_t *)stack) - 1;

    ptr->sreg = 0x80;
    ptr->r24 = (uint8_t)((uint16_t)cookie & 0xff); 
    ptr->r25 = (uint8_t)(((uint16_t)cookie >> 8) & 0xff); 
    ptr->pc  = (uint16_t)thr_func;

    return((archtype_t *)ptr);
}

void port_swap_req(void)
{
	/* force int0 external interrupt */
	GIMSK |= (1 << 6);
	GIFR  |= (1 << 6);
}

void port_init_machine(void)
{
	/* interrupts initally shutdown */
	GIMSK  = 0;
	SREG   = 0;

	/* enable timer and match engine */
	TCCR0A  = 0;
	TCCR0B  = 0x00;
	TIMSK  |= 0x02;
}


#if(K_ENABLE_TIMERS > 0)

void port_start_timer(archtype_t reload_val)
{
	TCNT0  = 0;
	OCR0A  = (reload_val  & 0xFF);
	TIMSK |= 0x10;
	TCCR0B = 0x07;
}

void port_timer_load_append(archtype_t append_val)
{
	OCR0A  = (reload_val  & 0xFF);	
}

extern uint32_t port_timer_halt(void)
{
	uint32_t ret = TCNT0;
	TCCR0B = 0;
	return(ret);
}

extern void port_timer_resume(void)
{
	TCCR0B = 0x07;
}


extern void timer_match_handler(void)
{
	extern tcb_t timer_tcb;
	kernel_irq_in();
	/* request timeline handling */
	thread_set_signals(&timer_tcb, K_TIMER_DISPATCH);
	kernel_irq_out();
}

extern void timer_ovf_handler(void)
{
	kernel_irq_in();
	kernel_irq_out();
}


#endif

uint8_t port_bit_fs_scan(archtype_t reg)
{
	/* clz not implemented for this architecture */
	uint8_t ret = k_status_ok;
	ULIPE_ASSERT(false);
	return(ret);
}

uint8_t port_bit_ls_scan(archtype_t reg)
{
	/* ctz is not as well */
	uint8_t ret=k_status_ok;
	ULIPE_ASSERT(false);
	return(ret);
}
#endif
