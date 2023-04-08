#pragma once

#include "Timer.h"

namespace LEDC
{
class Channel
{
public:
	struct Config {
		uint8_t gpio;
		bool invert;
		ledc_intr_type_t intr_type;
		uint32_t duty;
		int hpoint;
	};

	// Allocate next free channel
	Channel(Timer& timer) : timer(timer)
	{
		assign(timer.findUnusedChannel());
	}

	// Use specific channel
	Channel(Timer& timer, ledc_channel_t id) : timer(timer)
	{
		assign(id);
	}

	~Channel()
	{
		if(*this) {
			stop(0);
			timer.claimChannel(id, false);
		}
	}

	// Determine if channel is valid
	explicit operator bool() const
	{
		return id < LEDC_CHANNEL_MAX;
	}

	bool configure(const Config& config);

	bool setPin(uint8_t gpio)
	{
		return ledc_set_pin(gpio, timer.mode, id) == ESP_OK;
	}

	bool stop(uint32_t idleLevel)
	{
		return ledc_stop(timer.mode, id, idleLevel) == ESP_OK;
	}

	bool updateDuty()
	{
		return ledc_update_duty(timer.mode, id) == ESP_OK;
	}

	bool setDutyWithHpoint(uint32_t duty, uint32_t hpoint)
	{
		return ledc_set_duty_with_hpoint(timer.mode, id, duty, hpoint) == ESP_OK;
	}

	bool setDuty(uint32_t duty)
	{
		return ledc_set_duty(timer.mode, id, duty) == ESP_OK;
	}

	/* omit fades for now...
        bool setFade(uint32_t duty, ledc_duty_direction_t fade_direction, uint32_t step_num, uint32_t duty_cycle_num, uint32_t duty_scale);
        bool setFadeWithStep(uint32_t target_duty, uint32_t scale, uint32_t cycle_num);
        bool setFadeWithTime(uint32_t target_duty, int max_fade_time_ms);
        bool fadeStart(ledc_fade_mode_t fade_mode)
        bool fadeFuncInstall(int intr_alloc_flags);
        bool fadeFuncUninstall();
        bool ledc_isr_register(InterruptCallback fn, void *arg, int intr_alloc_flags, ledc_isr_handle_t *handle );
        */

protected:
	void assign(ledc_channel_t id)
	{
		if(id >= LEDC_CHANNEL_MAX || timer.isChannelUsed(id)) {
			return;
		}
		this->id = id;
		timer.claimChannel(id, true);
	}

	Timer& timer;
	ledc_channel_t id{LEDC_CHANNEL_MAX};
};

} // namespace LEDC
