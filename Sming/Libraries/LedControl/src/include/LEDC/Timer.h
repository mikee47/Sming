#pragma once

#include <cstdint>
#include <bitset>
#include <driver/ledc.h>

namespace LEDC
{
class Channel;

class Timer
{
public:
	// Allocate next free available timer
	Timer(ledc_mode_t mode)
	{
		assign(mode, findUnusedTimer(mode));
	}

	// Allocate specific timer
	Timer(ledc_mode_t mode, ledc_timer_t id)
	{
		assign(mode, id);
	}

	~Timer()
	{
		if(*this) {
			pause();
			usedTimers[mode][id] = false;
		}
	}

	// Determine if timer is valid
	explicit operator bool() const
	{
		return mode < LEDC_SPEED_MODE_MAX && id < LEDC_TIMER_MAX;
	}

	bool setFrequency(uint32_t freqHz)
	{
		return ledc_set_freq(mode, id, freqHz) == ESP_OK;
	}

	uint32_t getFrequency() const
	{
		return ledc_get_freq(mode, id);
	}

	bool configure(uint32_t clock_divider, uint32_t duty_resolution, ledc_clk_src_t clk_src);

	bool reset()
	{
		return ledc_timer_rst(mode, id) == ESP_OK;
	}

	bool pause()
	{
		return ledc_timer_pause(mode, id) == ESP_OK;
	}

	bool resume()
	{
		return ledc_timer_resume(mode, id) == ESP_OK;
	}

protected:
	friend class Channel;

	void assign(ledc_mode_t mode, ledc_timer_t id)
	{
		if(id >= LEDC_TIMER_MAX || usedTimers[mode][id]) {
			return;
		}
		this->mode = mode;
		this->id = id;
		usedTimers[mode][id] = true;
	}

	static ledc_timer_t findUnusedTimer(ledc_mode_t mode)
	{
		for(unsigned i = 0; i < LEDC_TIMER_MAX; ++i) {
			if(!usedTimers[mode][i]) {
				return ledc_timer_t(i);
			}
		}
		return LEDC_TIMER_MAX;
	}

	bool isChannelUsed(ledc_channel_t ch)
	{
		return !usedChannels[mode][ch];
	}

	void claimChannel(ledc_channel_t ch, bool claimed)
	{
		usedTimers[mode][ch] = claimed;
	}

	ledc_channel_t findUnusedChannel()
	{
		for(unsigned i = 0; i < LEDC_CHANNEL_MAX; ++i) {
			if(!usedChannels[mode][i]) {
				return ledc_channel_t(i);
			}
		}
		return LEDC_CHANNEL_MAX;
	}

	static std::bitset<LEDC_TIMER_MAX> usedTimers[];
	static std::bitset<LEDC_CHANNEL_MAX> usedChannels[];
	ledc_mode_t mode{LEDC_SPEED_MODE_MAX};
	ledc_timer_t id{LEDC_TIMER_MAX};
};

class LSTimer : public Timer
{
public:
	LSTimer() : Timer(LEDC_LOW_SPEED_MODE)
	{
	}

	LSTimer(ledc_timer_t id) : Timer(LEDC_LOW_SPEED_MODE, id)
	{
	}
};

#if SOC_LEDC_SUPPORT_HS_MODE
class HSTimer : public Timer
{
public:
	HSTimer() : Timer(LEDC_HIGH_SPEED_MODE)
	{
	}

	HSTimer(ledc_timer_t id) : Timer(LEDC_HIGH_SPEED_MODE, id)
	{
	}
};
#endif

} // namespace LEDC
