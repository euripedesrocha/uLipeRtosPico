# uLipeRtosPico
Simple preemptive-cooperative, realtime, multitask kernel made just for fun.
uLipeRtosPico is a subset of uLipeRTOS, but is small and powerful real time kernel, currently implemented for arm cortex M and AVR 8 bits processors.
The kernel uses a fully preemptive-cooperative schedule policy, and supports up to 32 priority levels for maximum flexibility.

# Low memory footprint:
  - 1.2KB of code with full modules enabled*; 
  - 80B of RAM data used by the kernel;	
  - fully static allocation of kernel objects less than 40 bytes per one(user controls ram usage during compile time);

  *built on GCC_ARM 5.2 with -Os option
  
# Main Features

- Real time, all functions has predictable execution time;
- Fast and predictable execution time context switching;
- Tickless optional operation;
- Static kernel object allocation;
- Supports up to 32 priority levels (0 - 31);
- Preemtpive kernel policy;
- Cooperative kernel policy with same priority threads;
- Thread signals with set, clear, any and match capabilities;
- Counting semaphores;
- Binary semaphores;
- Mutual exclusion semaphores with priority ceilling protocol;
- Message queues;
- Soft timers with tickless feature (hardware timer provided by user);
- Constant time, low & constant overhead fixed size memory pool (up to 1024 elements per pool);
- Workqueues, suitable form to defer asynchronous jobs and create event driven state machines;
- Unlimited kernel objects (limited by processor memory);
- Port file formed by two simple files in C and Assembly, simple to port;
- Glue header kernel, just put master file in your application and enjoy.

# Recommended processor resources

- 2KB of Code targeted memory(ROM);
- 128B of Data memory (RAM) or 372B of Data memory(RAM) in case of mempool use;


# Basic Usage

- uLipeRtosPico was built to be simple in its essence;
- Add the folders to the include paths: uLipeRtosPico;
- Add the folders as sources: uLipeRtosPico/picokernel and uLipeRtosPico/picokernel and uLipeRtosPico/arch;
- Create your own ulipe_rtos_kconfig.h with some properties or use the default config:

```


/* architecture definition */
#define ARCH_TYPE_ARM_CM0				1

 
/* architecture data width */
#define K_ARCH_MEM_WIDTH_WORD			1


/* general kernel configuration */
#define K_MINIMAL_STACK_VAL				16
#define K_TIMER_DISPATCHER_PRIORITY		4
#define K_TIMER_DISPATCHER_STACK_SIZE	256


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
