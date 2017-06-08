/**
 * 							ULIPE RTOS PICO
 *  @file k_raw_timer.h
 *
 *  @brief timer primitive kernel header file
 *
 */

#ifndef __K_RAW_TIMER_H
#define __K_RAW_TIMER_H


#if(K_ENABLE_TIMERS > 0)

/* timer commands */
#define K_TIMER_LOAD_FRESH		0x01
#define K_TIMER_DISPATCH 		0x02
#define K_TIMER_REFRESH			0x04

/* timer callback function type */
typedef void (*ktimer_callback_t) (void * t);


/* timer control block structure */
typedef struct ktimer{
	archtype_t load_val;
	archtype_t ;
	ktimer_callback_t cb;
	bool expired;
	bool created;
	bool running;
	k_work_list_t threads_pending;
	k_list_t timer_list_link;
}ktimer_t;




/**
 *  @fn TIMER_CONTROL_BLOCK_DECLARE()
 *  @brief declares a fully initialized control block for timer
 *  @param
 *  @return
 */
#define TIMER_CONTROL_BLOCK_DECLARE(name,load_value)		\
		ktimer_t name = {									\
			.timer_to_wait=load_value,						\
			.running=false,									\
			.expired=true,									\
			.threads_pending.bitmap=0,						\
			.created=false,									\
		}


/**
 *  @fn timer_start()
 *  @brief starts a timer counting
 *  @param
 *  @return
 */
k_status_t timer_start(ktimer_t *t);

/**
 *  @fn timer_poll()
 *  @brief check if a timer expired, blocks if not expired yet
 *  @param
 *  @return
 */
k_status_t timer_poll(ktimer_t *t);

/**
 *  @fn timer_set_callback()
 *  @brief sets a callback for execution when timer expires
 *  @param
 *  @return
 */
k_status_t timer_set_callback(ktimer_t *t, ktimer_callback_t cb);

/**
 *  @fn timer_set_load()
 *  @brief set load time value
 *  @param
 *  @return
 */
k_status_t timer_set_load(ktimer_t *t, archtype_t load_val);

#endif
#endif
