# uLipeRtosPico
Simple preemptive-cooperative, realtime, multitask kernel made just for fun.
uLipeRtosPico is a subset of uLipeRTOS, but is small and powerful real time kernel, currently implemented for arm cortex M0 architectures.
The kernel uses a fully preemptive-cooperative schedule policy, and supports up to 4 priority levels + 3 system high priorities for
high performance threads.

# Low memory footprint:
  - 1.2KB of code with full modules enabled*; 
  - 80B of RAM data used by the kernel;	
  - fully static allocation of kernel objects less than 40 bytes per one(user controls ram usage during compile time);

  *built on GCC_ARM 5.2 with -Os option
  
# Main Features

- Real time all functions are O(1), preemptive-cooperative microkernel;
- Fast context switching time, below to 100ns @ 50MHz processor clock;
- Tickless operation;
- Static kernel object allocation;
- Supports up to 4 priority levels, 3 sys prio for high perfomance tasks;
- Threads with same priorities will run in cooperative way;
- Thread signals with set, clear, any and match capabilities;
- Counting semaphores;
- Binary semaphores with optional priority ceilling offering mutex primitives;
- Message queues;
- Unlimited kernel objects (limited by processor memory);
- Port file formed by two simple files in C and Assembly, simple to port;
- Single header kernel, put on you application and enjoy.

# Recommended processor resources

- 2KB of Code targeted memory(ROM);
- 128B of Data memory (RAM)


# Basic Usage

- uLipeRtosPico was built to be simple in its essence;
- Add the folders to the include paths: uLipeRtosPico;
- Add the folders as sources: uLipeRtosPico/picokernel and uLipeRtosPico/picokernel and uLipeRtosPico/arch;
- Go to ulipe_rtos_kconfig.h and edit the following lines:

```

/* timer related */
#define K_MACHINE_CLOCK					48000000
#define K_TICKER_FREQUENCY				1000

/* architecture definition */
#define ARCH_TYPE_ARM_CM0				1
#define ARCH_TYPE_ARM_CM3_4_7				0

 
/* architecture data width */
#define K_ARCH_MEM_WIDTH_BYTE			0
#define K_ARCH_MEM_WIDTH_HALFWORD		0
#define K_ARCH_MEM_WIDTH_WORD			1
#define K_ARCH_MEM_WIDTH_DWORD			0


/* general kernel configuration */
#define K_MINIMAL_STACK_VAL				16
#define K_TIMER_DISPATCHER_PRIORITY			4
#define K_TIMER_DISPATCHER_STACK_SIZE			128
#define K_ENABLE_SEMAPHORE				0
#define K_ENABLE_MESSAGING				0
#define K_ENABLE_TIMERS					0


/* kernel debugging */
#define K_DEBUG						1

```

- Play witth the following demo:

```
#include "ulipe_rtos_pico.h"

THREAD_CONTROL_BLOCK_DECLARE(th1, 64, 3);
THREAD_CONTROL_BLOCK_DECLARE(th2, 64, 2);
THREAD_CONTROL_BLOCK_DECLARE(th3, 64, 2);
THREAD_CONTROL_BLOCK_DECLARE(th4, 64, 2);


static void test_thread_a(void *arg)
{
	uint32_t cntr = 0;
	for(;;){
		cntr += 1;
		(void)cntr;
		thread_set_prio(NULL, K_PRIORITY_LEVELS - 3);

		thread_yield();
	}
}


static void test_thread_b(void *arg)
{
	uint32_t cntr = 0;
	for(;;){
		cntr += 2;
		(void)cntr;
		uint32_t signals = thread_wait_signals(thread_get_current(), 0xA5, k_wait_match_any,NULL);
		thread_clr_signals(thread_get_current(), signals);
		(void)signals;

	}

}


static void test_thread_c(void *arg)
{
	uint32_t cntr = 0;
	for(;;){
		cntr += 3;
		(void)cntr;

		thread_set_signals(&th2, 0xA4);
		thread_yield();
	}

}


static void test_thread_d(void *arg)
{
	uint32_t cntr = 0;
	for(;;){
		cntr += 4;
		(void)cntr;

		thread_set_prio(&th1, 2);
		thread_yield();
	}
}


int main(void)
{

	/* Init function always need to be called at first time*/
	kernel_init();
	thread_create(&test_thread_a, NULL, &th1);
	thread_create(&test_thread_b, NULL, &th2);
	thread_create(&test_thread_c, NULL, &th3);
	thread_create(&test_thread_d, NULL, &th4);
	kernel_start();

	return 0;
}
```

- Build using your preferred IDE;
