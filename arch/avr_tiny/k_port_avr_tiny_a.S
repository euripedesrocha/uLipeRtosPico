#include "../../ulipe_rtos_kconfig.h"
#if(ARCH_TYPE_AVR_TINY> 0)

#include <avr/io.h>

;
;			ULIPE RTOS PICO
;	file k_port_avr_tiny_a.S
;   brief specific machine instruction level code
;
;



#define __SPL 		0x3d
#define __SPH  		0x3e
#define __SREG		0x3F

#if (ARCH_ENABLE_AVR_TINY_SPECS > 0)
	#define __EIMSK		0x3B
	#define __EIFR		0x3A
#else
	#define __EIMSK		0x1D
	#define __EIFR		0x1C
#endif


		;extern kernel variables
		.extern k_current_task
		.extern k_high_prio_task
		.extern k_running
		.extern timer_match_handler
		.extern timer_ovf_handler
		.extern irq_nesting
		.extern irq_counter

		;export symbols
		.global port_irq_lock
		.global port_irq_unlock
		.global port_from_isr
		.global port_start_kernel
		.global port_set_break
        .global port_swap
		.global INT0_vect 



		.section .text

<<<<<<< HEAD
=======

port_irq_lock:
		push r22
		lds  r22,irq_counter
		cpi  r22, #0
		brne port_irq_lock_0
		lds  r22,
		in	r24, __SREG
		cli
port_irq_lock_0:
		pop	r22
    	ret
;------------------------------------------------------------------------------------------------
port_irq_unlock:
		push r22
		lds  r22, irq_counter
		cpi	 r22, #0
		brne port_irq_unlock_0
		out __SREG,r24 
port_irq_unlock_0:
		pop	r22
    	ret
>>>>>>> ffe4ef66baf8e81422e3a5f4195057ce531f1a8f
;------------------------------------------------------------------------------------------------
port_from_isr:
		push r22
		ldi r24, #0
		lds	 r22, irq_counter
		cpi	 r22, #0
		breq port_from_isr_0
		ldi	 r24, #1
port_from_isr:
		pop	r22
    	ret

;--------------------------------------------------------------------------------------------------
port_set_break:
   		ret

;--------------------------------------------------------------------------------------------------
port_start_kernel:
		push r18
		push r19
		push r20
		push r21
		push r22
		push r23
		push r24
		push r25						; allocate workspace (we dont want to corrupt stack here)
		cli

		ldi r24, 0x01					
		sts k_running,r24				; kernel running!
		lds r24, k_high_prio_task		; obtains the first task ready to run and
		lds r25, k_high_prio_task + 1	; make it the current task
		sts k_current_task,r24			;	
		sts k_current_task	+ 1,r25		;
		movw r30, r24					;
		ld   r24, z+					;
		ld   r25, z						;
		out  __SPL, r24					; Load the first task context 
		out	 __SPH, r25					; it is resides on stack

		pop  r0							; restore the registers (which holds the initial context)
		pop  r1		
		pop  r2		
		pop  r3		
		pop  r4	
		pop  r5		
		pop  r6		
		pop  r7		
		pop  r8		
		pop  r9		
		pop  r10		
		pop  r11		
		pop  r12		
		pop  r13		
		pop  r14		
		pop  r15		
		pop  r16		
		pop  r17		
		pop  r18		
		pop  r19		
		pop  r20		
		pop  r21		
		pop  r22		
		pop  r23		
		pop  r24		
		pop  r25		
		pop  r26		
		pop  r27		
		pop  r28		
		pop  r29		
		pop  r30		
		pop  r31
		out  __SREG,r31
		pop  r31
		ret

;--------------------------------------------------------------------------------------------------
INT0_vect:
		push  r31
		in 	  r31, __SREG
		push  r31
		push  r30		
		push  r29		
		push  r28		
		push  r27		
		push  r26		
		push  r25		
		push  r24		
		push  r23		
		push  r22		
		push  r21		
		push  r20		
		push  r19		
		push  r18		
		push  r17		
		push  r16		
		push  r15		
		push  r14		
		push  r13		
		push  r12		
		push  r11		
		push  r10		
		push  r9		
		push  r8		
		push  r7		
		push  r6		
		push  r5		
		push  r4	
		push  r3		
		push  r2		
		push  r1		
		push  r0						; save all cpu regs			

#if (ARCH_ENABLE_AVR_TINY_SPECS > 0)
		in r24, __EIMSK
		andi r24, ~(1 << 6)
		out __EIMSK, r24 

		in r24, __EIFR
		andi r24, ~(1 << 6)
		out __EIFR, r24 
#else

		in r24, __EIMSK
		andi r24, 0xFE
		out __EIMSK, r24 

		in r24, __EIFR
		andi r24, 0xFE
		out __EIFR, r24 
#endif
		 
		in  r24, __SPL					; 	
		in	r25, __SPH					;
		lds r22, k_current_task			; Save the current context to its owner tcb
		lds r23, k_current_task + 1		;
		movw r30, r22					;
		st  z+,r24						;
		st  z, r25 						;

		lds r24, k_high_prio_task		; obtains the next ready to run task
		lds r25, k_high_prio_task + 1	; make it the current task
		sts k_current_task,r24			;	
		sts k_current_task	+ 1,r25		;
		movw r30, r24					;
		ld   r24, z+					;
		ld   r25, z						;
		out  __SPL,r24					; Loads the task context 
		out	 __SPH,r25					; 

		pop  r0							; restore all cpu regs 
		pop  r1		
		pop  r2		
		pop  r3		
		pop  r4	
		pop  r5		
		pop  r6		
		pop  r7		
		pop  r8		
		pop  r9		
		pop  r10		
		pop  r11		
		pop  r12		
		pop  r13		
		pop  r14		
		pop  r15		
		pop  r16		
		pop  r17		
		pop  r18		
		pop  r19		
		pop  r20		
		pop  r21		
		pop  r22		
		pop  r23		
		pop  r24		
		pop  r25		
		pop  r26		
		pop  r27		
		pop  r28		
		pop  r29		
		pop  r30		
		pop  r31
		out  __SREG,r31
		pop  r31

	    reti
;--------------------------------------------------------------------------------------------------
	
.end
#endif

