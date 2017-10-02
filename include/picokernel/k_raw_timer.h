/**
 * 							ULIPE RTOS PICO
 *  @file k_raw_timer.h
 *
 *  @brief timer primitive kernel header file
 *
 */

#ifndef __K_RAW_TIMER_H
#define __K_RAW_TIMER_H


#if(K_ENABLE_TICKER > 0)

/* timer commands, not used for user application  */
#define K_TIMER_LOAD_FRESH		0x01
#define K_TIMER_DISPATCH 		0x02
#define K_TIMER_REFRESH			0x04
#define K_TIMER_TICK			0x08

#if(K_ENABLE_TIMERS > 0)

/**
 * timer callback function type
 *
 * @param user_data - custom data passed by user
 * @param timer - instance of expired timer
 *
 * @return none
 *
 */
typedef void (*ktimer_callback_t) (void * user_data, void *timer);



/* timer control block structure */
typedef struct ktimer{
	uint32_t load_val;
	uint32_t timer_to_wait;
	ktimer_callback_t cb;
	void *user_data;
	bool expired;
	bool created;
	bool running;
	k_work_list_t threads_pending;
	k_list_t timer_list_link;
}ktimer_t;


/* kernel execution information */
typedef struct k_wakeup_info {
	tcb_t *next_thread_wake;
	ktimer_t *next_timer;
	uint32_t *tick_cntr;
}k_wakeup_info_t;


/**
 *  @fn TIMER_CONTROL_BLOCK_DECLARE()
 *  @brief declares a fully initialized control block for timer
 *
 *  @param name - name of initialized timer control structure
 *  @param load_value - initial load value (can be changed using timer API)
 *
 *  @return a ktimer_t structure initialized and ready to use
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
 *  @brief starts the specified timer to count
 *
 *  @param t - timer instance to be started
 *
 *  @return k_status_ok or error in case of invalid values
 *
 */
k_status_t timer_start(ktimer_t *t);


/**
 *  @fn timer_stop()
 *  @brief stops a timer to count and remove it from active timers list
 *
 *  @param t - timer to be stopped
 *
 *  @return k_status_ok or error in case of invalid values
 */
k_status_t timer_stop(ktimer_t *t);


/**
 *  @fn timer_poll()
 *  @brief check if a timer expired, blocks if not expired yet
 *
 *  @param t - timer to be polled
 *
 *  @return k_status_ok or error in case of invalid values
 */
k_status_t timer_poll(ktimer_t *t);


/**
 *  @fn timer_set_callback()
 *  @brief sets a callback for execution when timer expires
 *
 *  @param t - timer to be callback set
 *  @param cb - callback to be called when timer expires
 *  @param user_data - custom data to be passed to callback
 *
 *  @return k_status_ok or error in case of invalid values / timmer already running
 */
k_status_t timer_set_callback(ktimer_t *t, ktimer_callback_t cb, void *user_data);

/**
 *  @fn timer_set_load()
 *  @brief set counting time value
 *
 *  @param t - timer to have counting modified
 *  @param load_val - counting value
 *
 *  @return k_status_ok or error in case of invalid values / timmer already running
 */
k_status_t timer_set_load(ktimer_t *t, uint32_t load_val);
#endif


/**
 *  @fn ticker_timer_wait()
 *  @brief Sleeps the current thread by some ticks amount
 *
 *  @param ticks - ticks to sleep the current thread
 *
 *  @return k_status_ok or error in case of invalid values
 */ 
k_status_t ticker_timer_wait(uint32_t ticks);


/**
 *  @fn timer_get_tick_count()
 *  @brief gets the current tick count
 *
 *  @param none
 *
 *  @return current tick count value
 */
uint32_t timer_get_tick_count(void);




#endif
#endif
