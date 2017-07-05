#include "../../../ulipe_rtos_kconfig.h"
#if(ARCH_TYPE_QUARK_IAMCU > 0)
/**
 * 							ULIPE RTOS PICO
 *
 *  @file k_port_x86_iamcu_a.S
 *
 *  @brief specific x86 iamcu port instruction level file
 *
 *
 */
 			
	
#define PORT_SWAP_VECTOR	$0x20


		@external  kernel variables
		.extern k_current_task
		.extern k_high_prio_task
		.extern k_running
		.extern irq_counter


		@export symbols
		.global port_irq_lock
		.global port_irq_unlock
		.global port_from_isr
		.global port_start_kernel
		.global port_swap_req
		.global port_set_break
		.global	port_bit_fs_scan
		.global port_bit_ls_scan
		.global port_swap



		.section .text

port_irq_lock:							@ 
		pushl  	%ebp					@
		movl  	%ebp, %esp				@
		movl	-4(%ebp),%eax 			@
		cmpl	$0, irq_counter			@ is in a ISR?
		jne		port_irq_lock_0			@ no need to lock irq inside in a ISR
		pushfl 							@ save interrupt states
		cli								@
		popl 	%eax 					@ uses pop instruction making the current eflags
port_irq_lock_0:
		popl	%ebp
		ret 							@ to fall in eax reg


@------------------------------------------------------------------------------------------------
port_irq_unlock:
		pushl  	%ebp					@
		movl  	%ebp, %esp				@
		movl	-4(%ebp),%eax 			@
		cmpl	$0, irq_counter			@ is in a ISR?
		jne		port_irq_lock_0			@ no need to lock irq inside in a ISR
		testl   $0x200, %eax 			@ test if interrupts needs to be enabled
		jz		port_irq_unlock_0		@ only enable if it already was enabled
		seti   							@ before to lock it
port_irq_unlock_0:					
		popl    %ebp					@
		ret

@------------------------------------------------------------------------------------------------
port_from_isr:
		pushl  	%ebp					@
		movl  	%ebp, %esp				@
		movl	-4(%ebp),%eax 			@
		movl	$0, %eax				@
		cmpl	$0, irq_counter			@
		jne		port_from_isr_0			@
		movl	$1, %eax
port_from_isr_0:
		popl    %ebp					@
		ret

@--------------------------------------------------------------------------------------------------

#if (K_DEBUG > 0)
port_set_break:
		int     $3						@ vector $3 is used as breakpoint instruction
		nop								@ in IA-32 arch
		nop								@
		jmp		$						@

#endif

@--------------------------------------------------------------------------------------------------
port_bit_fs_scan:
		cmpl    $0, %eax 				@avoid unexpected result when parametes is 0
		jz		port_bit_fs_scan_0		@
		bsr 	%eax, %eax 				@
		jmp 	port_bit_fs_scan_cleanup@
port_bit_fs_scan_0:					
		movl	$0x10,%eax 				@

port_bit_fs_scan_cleanup:
		ret 

@--------------------------------------------------------------------------------------------------
port_bit_ls_scan:
		cmpl    $0, %eax 				
		jz		port_bit_ls_scan_0
		bsf 	%eax, %eax
		jmp 	port_bit_ls_scan_cleanup

port_bit_ls_scan_0:
		movl	$0x10,%eax

port_bit_ls_scan_cleanup:
		ret 


@--------------------------------------------------------------------------------------------------
port_start_kernel:
		pushl 	%ebp						@
		movl  	%ebp, %esp					@
		movl	k_high_prio_task, %ecx		@find the first task ready to run
		movl	(%ecx), %eax
		movl	(%eax), %esp				@marks os as running 
		movl	k_running, %eax		
		movb    $1,	(%eax)				
		movl 	k_current_task, %eax		@the hpt is now the current task
		movl	(%ecx), (%eax)				@
		popfl 								@setup initial eflags
		popal								@first context
		ret

@--------------------------------------------------------------------------------------------------
port_swap_req:
		int PORT_SWAP_VECTOR	 	@request a sw interrupt where the swap
		nop							@handler was previously installed
		nop
		ret


@--------------------------------------------------------------------------------------------------
port_swap:
		pushal								@ in iamcu architecture we need to store manually
		pushfl 								@ store also the current eflags
		movl	k_high_prio_task, %eax		@the IA-32 GPRs first 
		movl	k_current_task,	  %edx		@
		movl	(%edx), %ecx				@
		movl 	%esp,   (%ecx)				
		movl	(%edx), %ecx				@ restore the new task stack
		movl	(%eax), (%edx)
		movl	(%ecx), %esp
		popfl								@restore the incoming thread eflags 					
		popal								@then the incoming context
		iret
	.end
#endif
