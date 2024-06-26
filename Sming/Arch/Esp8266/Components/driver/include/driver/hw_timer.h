/****
 * Sming Framework Project - Open Source framework for high efficiency native ESP8266 development.
 * Created 2015 by Skurydin Alexey
 * http://github.com/SmingHub/Sming
 * All files of the Sming Core are provided under the LGPL v3 license.
 *
 * hw_timer.h
 *
 ****/

#pragma once

#include <esp_systemapi.h>

#define HW_TIMER_BASE_CLK APB_CLK_FREQ

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup hw_timer Hardware Timer Driver
 * @ingroup drivers
 * @{
 */

/*************************************
 *
 * FRC1 timer
 *
 * This is a 23-bit countdown timer
 *
 *************************************/

/**
 * @brief Maximum timer interval in ticks
 * @note The corresponding time interval depends on the prescaler in use:
 *
 * 		/1 - 0.1048s
 * 		/16 - 1.677s
 * 		/256 - 26.84s
 */
#define MAX_HW_TIMER1_INTERVAL 0x7fffff

/**
 * @brief Minimum hardware interval in microseconds
 * @note Attempting to use repetitive interrupts below this level can lead
 * to system instabliity and lockups, due to the software overhead in servicing
 * the interrupts.
 */
#define MIN_HW_TIMER1_INTERVAL_US 50U

typedef void (*hw_timer_callback_t)(void* arg);

typedef enum {
	TIMER_CLKDIV_1 = 0,
	TIMER_CLKDIV_16 = 4,
	TIMER_CLKDIV_256 = 8,
} hw_timer_clkdiv_t;

typedef enum {
	TIMER_EDGE_INT = 0,  // edge interrupt
	TIMER_LEVEL_INT = 1, // level interrupt
} hw_timer_intr_type_t;

typedef enum {
	TIMER_FRC1_SOURCE = 0,
	TIMER_NMI_SOURCE = 1,
} hw_timer_source_type_t;

/**
 * @brief Attach an interrupt for the timer
 * @param source_type
 * @param callback Callback function invoked via timer interrupt
 * @param arg Passed to callback function
 */
void IRAM_ATTR hw_timer1_attach_interrupt(hw_timer_source_type_t source_type, hw_timer_callback_t callback, void* arg);

/**
 * @brief Enable the timer
 * @param div
 * @param intr_type
 * @param auto_load true for repeating timer, false for one-shot
 *
 * Note: With one-shot timer application callback must stop the timer when it is no longer required.
 * This is to reduce callback latency.
 * If this is not done, timer will trigger again when timer counter wraps around to 0.
 * For /16 divisor this is only 1.7s.
 */
inline void IRAM_ATTR hw_timer1_enable(hw_timer_clkdiv_t div, hw_timer_intr_type_t intr_type, bool auto_load)
{
#define FRC1_ENABLE_TIMER BIT7
#define FRC1_AUTO_LOAD BIT6

	uint32_t ctrl = (div & 0x0C) | (intr_type & 0x01) | FRC1_ENABLE_TIMER;
	if(auto_load) {
		ctrl |= FRC1_AUTO_LOAD;
	}

	WRITE_PERI_REG(FRC1_CTRL_ADDRESS, ctrl);
	TM1_EDGE_INT_ENABLE();
	ETS_FRC1_INTR_ENABLE();
}

/**
 * @brief Set the timer interval
 * @param ticks
 */
__forceinline void IRAM_ATTR hw_timer1_write(uint32_t ticks)
{
	WRITE_PERI_REG(FRC1_LOAD_ADDRESS, ticks);
}

/**
 * @brief Disable the timer
 */
__forceinline void IRAM_ATTR hw_timer1_disable(void)
{
	TM1_EDGE_INT_DISABLE();
	ETS_FRC1_INTR_DISABLE();
}

/**
 * @brief Detach interrupt from the timer
 */
void hw_timer1_detach_interrupt(void);

/**
 * @brief Get timer1 count
 * @retval uint32_t Current count value, counts from initial value down to 0
 */
__forceinline uint32_t hw_timer1_read(void)
{
	return READ_PERI_REG(FRC1_COUNT_ADDRESS);
}

/*************************************
 *
 * FRC2 timer
 *
 * This is a 32-bit count-up timer
 *
 *************************************/

#ifdef USE_US_TIMER
#define HW_TIMER2_CLKDIV TIMER_CLKDIV_16
#else
#define HW_TIMER2_CLKDIV TIMER_CLKDIV_256
#endif

#define HW_TIMER2_CLK (HW_TIMER_BASE_CLK >> HW_TIMER2_CLKDIV)

/**
 * @brief Read current timer2 value
 * @retval uint32_t
 */
__forceinline uint32_t hw_timer2_read(void)
{
	return READ_PERI_REG(FRC2_COUNT_ADDRESS);
}

/**
 * @brief Set timer2 alarm count value
 * @param ticks
 * @note For internal use ONLY; used by software timers
 */
__forceinline void hw_timer2_set_alarm(uint32_t ticks)
{
	WRITE_PERI_REG(FRC2_ALARM_ADDRESS, ticks);
}

/**
 * @brief Initialise hardware timers
 * @note Called by startup code
 */
void hw_timer_init(void);

/** @} */

#ifdef __cplusplus
}
#endif
