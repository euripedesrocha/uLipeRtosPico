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
#if (ARCH_ENABLE_AVR_TINY_SPECS > 0)	
	GIMSK |= (1 << 6);
	GIFR  |= (1 << 6);

#else
	/* force int0 external interrupt */
	EIMSK |= (1 << INTF0);
	EIFR  |= (1 << INTF0);
#endif
}

void port_init_machine(void)
{

#if (ARCH_ENABLE_AVR_TINY_SPECS > 0)
	GIMSK = 0;
	GIFR  = 0;
#else	
	/* interrupts initally shutdown */
	EIMSK = 0;
	EIFR  = 0;
#if(K_ENABLE_TIMERS > 0)
	/* enable timer and match engine */
	TCCR1A  = 0;
	TCCR1B  = 0x00;
	TIMSK1  = (1 << OCIE1A) | (1 << TOIE1); 
#if (K_ENABLE_TICKER > 0)
	#if !defined(K_MACHINE_CLOCK) || !defined(K_TICKER_RATE)
	#error "The SoC clock and ticker rate needs to be defined to use tick feature!"
	#endif

	TCCR0A  = 0;
	TCCR0B  = 0x00;
	TIMSK0  = (1 << OCIE0A) | (1 << TOIE0); 
	OCR0A = K_MACHINE_CLOCK / (K_TICKER_RATE * 1024);
	TCCR0B  = 0x07;	

#endif
#endif
#endif


}

archtype_t port_irq_lock(void) 
{
	extern uint32_t irq_lock_nest;
	if(!irq_lock_nest)
		cli();

	irq_lock_nest++;
	return 0;
}


void port_irq_unlock(archtype_t key)
{
	extern uint32_t irq_lock_nest;
	if(irq_lock_nest > 0) {
		irq_lock_nest--;
		if(!irq_lock_nest)
			sei();
	}
	(void)key;
}

#if(K_ENABLE_TIMERS > 0) && !defined(ARCH_ENABLE_AVR_TINY_SPECS)

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


#if (K_ENABLE_TICKER > 0)

void timer_tick_handler(void)
{
	extern tcb_t timer_tcb;
	thread_set_signals(&timer_tcb, K_TIMER_TICK);	
}


ISR(TIMER0_COMPA_vect)
{
	kernel_irq_in();	
	timer_tick_handler();
	kernel_irq_out();
}
#endif

uint8_t port_bit_fs_scan(archtype_t x)
{
	/* clz not implemented for this architecture */
	uint8_t ret = 32;

	if(!x)
		goto cleanup;


	static uint8_t const clz_lkup[] = {
		32, 31, 30, 30, 29, 29, 29, 29,
		28, 28, 28, 28, 28, 28, 28, 28
	};

    uint32_t n;
	
    /*
     * Scan if bit is in top word
     */
    if (x >= (1 << 16)) {
		if (x >= (1 << 24)) {
			if (x >= (1 << 28)) {
				n = 28;
			}
			else {
				n = 24;
			}
		}
		else {
			if (x >= (1 << 20)) {
				n = 20;
			}
			else {
				n = 16;
			}
		}
    }
    else {
        /* now scan if the bit is on rightmost byte */
		if (x >= (1 << 8)) {
			if (x >= (1 << 12)) {
				n = 12;
			}
			else {
				n = 8;
			}
		}
        else {
            if (x >= (1 << 4)) {
                n = 4;
            }
            else {
                n = 0;
            }
        }
    }

	ret = (uint8_t)(clz_lkup[x >> n] - n);
cleanup:
	return(ret);
}

uint8_t port_bit_ls_scan(archtype_t arg)
{
	/* ctz is not as well */
	return(31 - port_bit_fs_scan(arg & -arg));
}



#endif

