/*
 * This implementation mimics the behaviour of the ESP8266 Non-OS SDK timers,
 * using Timer2 as the reference (which is _not_ in microseconds!)
 *
 * The ESP32 IDF contains more sophisticated timer implementations, but also
 * this same API which it refers to as the 'legacy' timer API.
 */

#pragma once

#include <c_types.h>

// Disarmed
#define OS_TIMER_DEFAULT()                                                                                             \
	{                                                                                                                  \
		.timer_next = (os_timer_t*)-1,                                                                                 \
	}

#ifdef __cplusplus
extern "C" {
#endif

typedef void os_timer_func_t(void* timer_arg);

/**
 * @brief This is the structure used by the Espressif timer API
 * @note This is used as an element in a linked list
 * The Espressif implementation orders the list according to next expiry time.
 * os_timer_setfn and os_timer_disarm set timer_next to -1
 * When expired, timer_next is 0
 */
struct os_timer_t {
	/// If disarmed, set to -1, otherwise points to the next queued timer (or NULL if last in the list)
	struct os_timer_t* timer_next;
	/// Set to the next Timer2 count value when the timer will expire
	uint32_t timer_expire;
	/// 0 if this is a one-shot timer, otherwise defines the interval in Timer2 ticks
	uint32_t timer_period;
	/// User-provided callback function pointer
	os_timer_func_t* timer_func;
	/// Argument passed to the callback function
	void* timer_arg;
};

void os_timer_arm_ticks(os_timer_t* ptimer, uint32_t ticks, bool repeat_flag);

void os_timer_arm(os_timer_t* ptimer, uint32_t time, bool repeat_flag);
void os_timer_arm_us(os_timer_t* ptimer, uint32_t time, bool repeat_flag);

void os_timer_disarm(os_timer_t* ptimer);
void os_timer_setfn(os_timer_t* ptimer, os_timer_func_t* pfunction, void* parg);

static inline uint64_t os_timer_expire(const os_timer_t* ptimer)
{
	if(ptimer == nullptr || intptr_t(ptimer->timer_next) == -1) {
		return 0;
	}
	return ptimer->timer_expire;
}

void os_timer_done(os_timer_t* ptimer);

/**
 * @brief Hook function to service timers
 * @retval int Milliseconds until next timer due, -1 if none
 */
int host_service_timers();

#ifdef __cplusplus
}
#endif
