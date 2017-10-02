/*
===============================================================================
 Name        : work_proj.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/

#if defined (__USE_LPCOPEN)
#if defined(NO_BOARD_LIB)
#include "chip.h"
#else
#include "board.h"
#endif
#endif

#include <cr_section_macros.h>

#if defined (__MULTICORE_MASTER_SLAVE_M0SLAVE) || \
    defined (__MULTICORE_MASTER_SLAVE_M4SLAVE)
#include "boot_multicore_slave.h"
#endif


#include "ulipe_rtos_pico.h"


THREAD_CONTROL_BLOCK_DECLARE(t1, 64, 4);
THREAD_CONTROL_BLOCK_DECLARE(t2, 64, 5);
THREAD_CONTROL_BLOCK_DECLARE(t3, 64, 7);

THREAD_CONTROL_BLOCK_DECLARE(t4, 64, 11);
THREAD_CONTROL_BLOCK_DECLARE(t5, 64, 13);
THREAD_CONTROL_BLOCK_DECLARE(t6, 64, 9);


SEMAPHORE_BLOCK_DECLARE(s1,0,1);
SEMAPHORE_BLOCK_DECLARE(s2,0,1);
SEMAPHORE_BLOCK_DECLARE(s3,0,1);

MEMPOOL_DECLARE(mem1, 64, 256);

TIMER_CONTROL_BLOCK_DECLARE(tm1, 2000);
TIMER_CONTROL_BLOCK_DECLARE(tm2, 1542);
TIMER_CONTROL_BLOCK_DECLARE(tm3, 685);

MUTEX_BLOCK_DECLARE(mtx1);
WQUEUE_CONTROL_BLOCK_DECLARE(mywq, 16);

unsigned char *ptr[256] = {0};

struct job {
	ksema_t *sem;
	wqueue_job_t job;
};


static void deferred_handler(wqueue_job_t *j)
{
	struct job* trigger = CONTAINER_OF(j, struct job, job);
	if(trigger) {
		semaphore_give(trigger->sem, 1);
	}

	k_block_free(&mem1, trigger);
}


static void tm1_handler(void *user_data, void *timer)
{
	(void)timer;

	struct job *j = k_block_alloc(&mem1);
	ulipe_assert(j != NULL);
	j->job.handler = deferred_handler;
	j->sem = (ksema_t *)user_data;
	wqueue_submit(&mywq, &j->job);

}


static void tm2_handler(void *user_data, void *timer)
{
	(void)timer;
	ksema_t *s = (ksema_t *)user_data;
	ulipe_assert(s != NULL);
	semaphore_give(s, 1);
}

static void tm3_handler(void *user_data, void *timer)
{
	(void) timer;
	ksema_t *s = (ksema_t *)user_data;
	ulipe_assert(s != NULL);
	semaphore_give(s, 1);
}


static void t1_task(void *arg)
{
	uint32_t cntr = (uint32_t) arg;

	timer_set_callback(&tm1, tm1_handler, &s1);
	timer_start(&tm1);


	for(;;) {
		semaphore_take(&s1);
		cntr += 4;
		timer_start(&tm1);
		//ticker_timer_wait(655);
	}
}

static void t2_task(void *arg)
{
	uint32_t cntr = (uint32_t) arg;

	timer_set_callback(&tm2, tm2_handler, &s2);
	timer_start(&tm2);


	for(;;) {
		semaphore_take(&s2);
		cntr += 4;
		timer_start(&tm2);

	}

}

static void t3_task(void *arg)
{
	uint32_t cntr = (uint32_t) arg;



	for(;;) {
		cntr += 4;

		for(int i = 0; i < 128; i++){
			ptr[i] = k_block_alloc(&mem1);
			ulipe_assert(ptr[i] != NULL);
			strcpy(ptr[i], "memory allocated!");
		}


		for(int i = 0; i < 128; i++){
			k_block_free(&mem1, ptr[i]);
		}


		for(int i = 0; i < 128; i++){
			ptr[i + 128] = k_malloc(32);
			ulipe_assert(ptr[i + 128] != NULL);
			strcpy(ptr[i + 128], "memory heap allocated!");
		}


		for(int i = 0; i < 128; i++){
			k_free(ptr[i + 128]);
		}



		ticker_timer_wait(2560);
	}
}

static char cbuffer[128] = {0};


static void t4_task(void *arg)
{


	for(;;) {
		mutex_take(&mtx1, false);
		strcpy(cbuffer, "thread 4 is owner of mutex! \n\r");
		mutex_give(&mtx1);
		ticker_timer_wait(563);

	}

}
static void t5_task(void *arg)
{


	for(;;) {
		mutex_take(&mtx1, false);
		strcpy(cbuffer, "thread 5 is owner of mutex! \n\r");
		mutex_give(&mtx1);
		ticker_timer_wait(5698);
	}

}
static void t6_task(void *arg)
{

	for(;;) {
		mutex_take(&mtx1, false);
		strcpy(cbuffer, "thread 6 is owner of mutex! \n\r");
		ticker_timer_wait(879);
		mutex_give(&mtx1);
	}

}



int main(void) {

#if defined (__USE_LPCOPEN)
    // Read clock settings and update SystemCoreClock variable
    SystemCoreClockUpdate();
#if !defined(NO_BOARD_LIB)
#if defined (__MULTICORE_MASTER) || defined (__MULTICORE_NONE)
    // Set up and initialize all required blocks and
    // functions related to the board hardware
    Board_Init();
#endif
    // Set the LED to the state of "On"
    Board_LED_Set(0, true);
#endif
#endif

#if defined (__MULTICORE_MASTER_SLAVE_M0SLAVE) || \
    defined (__MULTICORE_MASTER_SLAVE_M4SLAVE)
    boot_multicore_slave();
#endif

    // TODO: insert code here
    kernel_init();


    //create three dummy threads
    thread_create(t1_task, 0, &t1);
    thread_create(t2_task, 4, &t2);
    thread_create(t3_task, 8, &t3);


    thread_create(t4_task, 0, &t4);
    thread_create(t5_task, 4, &t5);
    thread_create(t6_task, 8, &t6);

    wqueue_init(&mywq);


    kernel_start();

    // Force the counter to be placed into memory
    volatile static int i = 0 ;
    // Enter an infinite loop, just incrementing a counter
    while(1) {
        i++ ;
    }
    return 0 ;
}
