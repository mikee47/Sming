#include "bl.h"
#include "driver/ledc.h"
#include <Digital.h>

PWMBase::~PWMBase()
{
	ledc_stop(mode, ledc_channel_t(_channel), 0);
	pinMode(_pin, INPUT);
}

void PWMBase::begin()
{
	ledc_timer_config_t ledc_timer = {
		.speed_mode = mode,
		.duty_resolution = LEDC_TIMER_8_BIT,
		.timer_num = timerNum,
		.freq_hz = 12000,
		.clk_cfg = LEDC_AUTO_CLK, // Auto select the source clock
	};
	ledc_timer_config(&ledc_timer);

	ledc_channel_config_t ledc_channel = {
		.gpio_num = _pin,
		.speed_mode = mode,
		.channel = ledc_channel_t(_channel),
		.intr_type = LEDC_INTR_DISABLE,
		.timer_sel = timerNum,
		.duty = 100,
		.hpoint = 0,
	};
	ledc_channel_config(&ledc_channel);
	ledc_set_duty(mode, ledc_channel_t(_channel), 0);
}

void PWMBase::adjust(uint8_t level)
{
	ledc_set_duty(mode, ledc_channel_t(_channel), level);
}

void PWMToneBase::onec(unsigned duration)
{
	ledc_set_freq(mode, timerNum, _freq);
	_tick.initializeMs(
		duration,
		[](void* param) {
			auto self = static_cast<PWMToneBase*>(param);
			ledc_stop(self->mode, ledc_channel_t(self->_channel), 0);
		},
		this);
	_tick.startOnce();
}
