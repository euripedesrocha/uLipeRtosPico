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
#include <avr/io.h>
#include <avr/interrupt.h>



archtype_t *port_create_stack_frame(archtype_t *stack, thread_t thr_func, void *cookie)
{
    avr_tiny_xcpt_contents_t *ptr = ((avr_tiny_xcpt_contents_t *)stack);
	--ptr;

    ptr->sreg = 0x80;
	ptr->r0  = 0x0;	
	ptr->r4  = 0x44;
	ptr->r5  = 0x55;	
    ptr->r24 = (uint8_t)((uint16_t)cookie & 0xff); 
    ptr->r25 = (uint8_t)(((uint16_t)cookie >> 8) & 0xff); 
    ptr->pcl  = ((uint16_t)thr_func) & 0xff ;
    ptr->pch  = ((uint16_t)thr_func >> 8) & 0xff ;
    return((archtype_t *)ptr);
}

void port_swap_req(void)
{
	/* force int0 external interrupt */
	EIMSK |= (1 << INTF0);
	EIFR  |= (1 << INTF0);
}

void port_init_machine(void)
{
	/* interrupts initally shutdown */
	EIMSK = 0;
	EIFR  = 0;
#if(K_ENABLE_TIMERS > 0)
	/* enable timer and match engine */
	TCCR1A  = 0;
	TCCR1B  = 0x00;
	TIMSK1  = (1 << OCIE1A) | (1 << TOIE1); 
#endif
}


#if(K_ENABLE_TIMERS > 0)

void port_start_timer(uint32_t reload_val)
{
	TCNT1  = 0;
	OCR1A  = (reload_val  & 0xFFFF);
	TIMSK1  = (1 << OCIE1A) | (1 << TOIE1); 
	TCCR1B = 0x05;
}

void port_timer_load_append(uint32_t append_val)
{
	OCR1A  = (append_val  & 0xFFFF);	
}

uint32_t port_timer_halt(void)
{
	TIMSK1  = 0; 
	TCCR1B = 0;
	uint32_t ret = TCNT1;
	return(ret);
}

void port_timer_resume(void)
{
	TIMSK1  = (1 << OCIE1A) | (1 << TOIE1); 
	TCCR1B = 0x05;
}


void timer_match_handler(void)
{
	extern tcb_t timer_tcb;
	/* request timeline handling */
	port_timer_halt();
	thread_set_signals(&timer_tcb, K_TIMER_DISPATCH);
}

void timer_ovf_handler(void)
{
	kernel_irq_in();
	kernel_irq_out();
}


ISR(TIMER1_COMPA_vect)
{
	kernel_irq_in();	
	timer_match_handler();
	kernel_irq_out();
}

ISR(TIMER1_OVF_vect)
{
	(void)0;
	//timer_ovf_handler();
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
