/**
 * 							ULIPE RTOS PICO
 *  @file k_raw_timer.h
 *
 *  @brief timer primitive kernel header file
 *
 */

#include "ulipe_rtos_pico.h"


#if(K_ENABLE_TIMERS > 0)

/* static variables */
static k_list_t k_timed_list;



/** private functions */


/**
 *  @fn timer_period_sort()
 *  @brief	sort all active timers on list to find which has the least time to wait
 *  @param
 *  @return
 */
static ktimer_t *timer_period_sort(k_list_t *tlist)
{
	ktimer_t *ret = NULL;
	return(ret);
}


/** public functions */
k_status_t timer_start(ktimer_t *t)
{
	k_status_t ret = k_status_ok;
	archtype_t key;

	if(t == NULL) {
		ret = k_status_invalid_param;
		goto cleanup;
	}



cleanup:
	return(ret);

}

k_status_t timer_poll(ktimer_t *t, ktimer_opt_t opt)
{
	k_status_t ret = k_status_ok;
	archtype_t key;

	if(t == NULL) {
		ret = k_status_invalid_param;
		goto cleanup;
	}



cleanup:
	return(ret);

}

k_status_t timer_set_callback(ktimer_t *t, ktimer_callback_t cb)
{
	k_status_t ret = k_status_ok;
	archtype_t key;

	if(t == NULL) {
		ret = k_status_invalid_param;
		goto cleanup;
	}


cleanup:
	return(ret);
}

k_status_t timer_set_load(ktimer_t *t, archtype_t load_val)
{
	k_status_t ret = k_status_ok;
	archtype_t key;

	if(t == NULL) {
		ret = k_status_invalid_param;
		goto cleanup;
	}

cleanup:
	return(ret);
}

k_status_t timer_stop(ktimer_t *t)
{
	k_status_t ret = k_status_ok;
	archtype_t key;

	if(t == NULL) {
		ret = k_status_invalid_param;
		goto cleanup;
	}


cleanup:
	return(ret);
}

#endif