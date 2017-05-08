@
@			ULIPE RTOS PICO
@	file k_port_cm0_a.S
@   brief specific machine instruction level code
@
@


#include "../../ulipe_rtos_kconfig.h"
#if(ARCH_TYPE_ARM_CM0 > 0)

		.thumb
		.syntax unified

		@extern kernel variables
		.extern k_current_task
		.extern k_high_prio_task
		.extern k_running


		@export symbols
		.global port_irq_lock
		.global port_irq_unlock
		.global port_from_isr
		.global port_start_kernel
		.global port_set_break
		.global SVC_Handler
		.global PendSV_Handler



		.section .text


		.thumb_func
port_irq_lock:
		mrs 	r0, ipsr
		cmp		r0, #0
		bne 	port_irq_lock_0
		mrs 	r0, primask
		cpsid 	i
port_irq_lock_0:
		bx		lr

@------------------------------------------------------------------------------------------------
		.thumb_func
port_irq_unlock:
		mrs 	r0, ipsr
		cmp		r0, #0
		bne		port_irq_unlock_0
		msr 	primask,r0
port_irq_unlock_0:
		bx 		lr

@------------------------------------------------------------------------------------------------
		.thumb_func
port_from_isr:
		mrs 	r0, ipsr			@ IPSR holds if processor is currenttly attending a isr
		cmp 	r0, #0				@ interrupt, otherwise, we return true, is in a isr
		beq     port_from_isr_0
		movs	r0, #1
port_from_isr_0:
		bx		lr

@--------------------------------------------------------------------------------------------------
		.thumb_func
port_set_break:
		nop
		nop
		cpsid 	I
		bkpt  	#0
		b		.

@--------------------------------------------------------------------------------------------------
		.thumb_func
port_start_kernel:
		movs 	r0, #0				@to start kernel we need to unmask the interrupts first
		msr		primask,r0			@
		svc		#0					@trigger a software interrupt
		nop							@the nops acts a hold time to svc reach the m0 core avoiding
		nop							@the kernel_start function to return and cause a false error
		nop							@condition
		bx		lr
@--------------------------------------------------------------------------------------------------

@
@	SVC_Handler()
@	brief: performs the first context switch and gives the application control to kernel
@
		.thumb_func
SVC_Handler:
		movs 	r0, #0x02			@
		msr  	control, r0			@ uses psp for now.
		isb							@ flushes the pipeline
		ldr 	r0,=k_high_prio_task@
		ldr 	r1,=k_current_task	@
		ldr 	r2, [r0]			@
		ldr 	r2, [r2]			@ takes the first task stack:

		ldmia 	r2!, {r4 - r7}		@ pops the first sw context
	    mov  	r8,r4
	    mov  	r9,r5
	    mov  	r10,r6
	    mov  	r11,r7
   	    ldmia 	r2!, {r4 - r7}
		msr 	psp, r2				@ the hardware deals with remaining context

		ldr 	r0,[r0]				@
		str 	r0,[r1]				@ the current task is the high prio task

		ldr   	r0, =k_running		@
		movs  	r1, #1				@ os is running
		strb  	r1, [r0]			@
		movs 	r2, #4
		mov 	r1, lr				@ensure correct EXC_RETURN address
		orrs 	r1,r1, r2
		mov 	lr, r1
		bx		lr
@-------------------------------------------------------------------------------------------------

@
@	PendSV_Handler
@	brief: performs in instruction level context switching action when required by kernel
@
		.thumb_func
PendSV_Handler:
		ldr 	r0,=k_current_task	@
		ldr 	r1,=k_high_prio_task@
		ldr 	r2, [r0]			@ takes the current task stack:
		mrs 	r3, psp				@ takes the current stack pointer
		subs 	r3, #32				@
	    str   	r3, [r2]			@
		stmia 	r3!, {r4 - r7}		@ save the software context
	    mov  	r4,r8
	    mov  	r5,r9
	    mov  	r6,r10
	    mov  	r7,r11
	    stmia 	r3!, {r4 - r7}		@ save the software context

		ldr 	r2,[r1]				@
		ldr 	r2,[r2]				@ takes the high prio task stk pointer
	    adds 	r2, #16				@ takes first the high registers
	    ldmia 	r2!, {r4 - r7}		@ pops the  sw context
	    mov  	r8,r4
	    mov  	r9,r5
	    mov  	r10,r6
	    mov  	r11,r7
		msr 	psp, r2				@ the hardware deals with remaining context
		subs 	r2, #32				@ go back and restore the low registers
	    ldmia 	r2!, {r4 - r7}

		ldr 	r2, [r1]			@
		str 	r2, [r0]			@ the high prio task is the current task
		movs 	r2, #4
		mov 	r1, lr
		orrs 	r1,r1, r2           @
		mov 	lr, r1
		bx		lr					@ the return depennds of current task stack contents


.end
#endif
