#include "../../ulipe_rtos_kconfig.h"
#if(ARCH_TYPE_ARM_CM3_4_7 > 0)
@
@			ULIPE RTOS PICO
@	file k_port_cm3_4_7_a.S
@   brief specific machine instruction level code
@
@



		.thumb
		.syntax unified

		@extern kernel variables
		.extern k_current_task
		.extern k_high_prio_task
		.extern k_running
		.extern irq_nesting

		@export symbols
		.global port_irq_lock
		.global port_irq_unlock
		.global port_from_isr
		.global port_start_kernel
		.global port_set_break
		.global port_bit_fs_scan
		.global port_bit_ls_scan
		.global SVC_Handler
		.global PendSV_Handler



		.section .text


		.thumb_func
port_irq_lock:
		ldr 	r1, =irq_lock_nest
		ldr		r2, [r1]
		cmp		r2, #0
		bne		port_irq_lock_0
		cpsid   I
port_irq_lock_0:
		adds r2, #1
		str  r2, [r1]
		bx		lr

@------------------------------------------------------------------------------------------------
		.thumb_func
port_irq_unlock:
		ldr 	r1, =irq_lock_nest
		ldr		r2, [r1]
		subs	r2, #1
		cmp		r2, #0
		bne		port_irq_unlock_0
		cpsie   I
port_irq_unlock_0:
		str  r2, [r1]
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
port_bit_fs_scan:
		clz r0, r0
		bx  lr

@--------------------------------------------------------------------------------------------------
		.thumb_func
port_bit_ls_scan:
		rbit r0, r0
		clz  r0, r0
		bx   lr

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

		ldmia 	r2!, {r4 - r11}		@ pops the first sw context
		msr	 	psp, r2				@ the remainning context is dealt by hardware

		ldr 	r0,[r0]				@
		str 	r0,[r1]				@ the current task is the high prio task

		ldr   	r0, =k_running		@
		movs  	r1, #1				@ os is running
		strb  	r1, [r0]			@
		orr   	lr,lr, #0x04		@ensures the correct EXC_RETURN
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
		stmdb 	r3!, {r4 - r11}		@ save the software context
		str   	r3, [r2]			@

		ldr 	r2,[r1]				@
		ldr 	r2,[r2]				@ takes the high prio task stk pointer
		ldmia 	r2!, {r4-r11}		@ pops  the new software saved context
		msr 	psp, r2				@ the hardware deals with remaining context

		ldr 	r2, [r1]			@
		str 	r2, [r0]			@ the high prio task is the current task

		orr 	lr,lr, #0x04        @
		bx		lr					@ the return depennds of current task stack contents

  
.end
#endif
