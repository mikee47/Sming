#ifndef __BL_H
#define __BL_H
#include <Arduino.h>
#include <SimpleTimer.h>
#include "driver/ledc.h"

class PWMBase
{
public:
	static constexpr ledc_timer_t timerNum{LEDC_TIMER_0};

	PWMBase(uint8_t pin, uint8_t channel)
	{
		_pin = pin;
		_channel = ledc_channel_t(channel);
	}

	virtual ~PWMBase()
	{
		ledc_stop(LEDC_LOW_SPEED_MODE, _channel, 0);
		pinMode(_pin, INPUT);
	}

	virtual void begin()
	{
		ledc_timer_config_t ledc_timer = {
			.speed_mode = LEDC_LOW_SPEED_MODE,
			.duty_resolution = LEDC_TIMER_8_BIT,
			.timer_num = timerNum,
			.freq_hz = 12000,
			.clk_cfg = LEDC_AUTO_CLK, // Auto select the source clock
		};
		ledc_timer_config(&ledc_timer);

		ledc_channel_config_t ledc_channel = {
			.gpio_num = _pin,
			.speed_mode = LEDC_LOW_SPEED_MODE,
			.channel = _channel,
			.intr_type = LEDC_INTR_DISABLE,
			.timer_sel = timerNum,
			.duty = 100,
			.hpoint = 0,
		};
		ledc_channel_config(&ledc_channel);
		ledc_set_duty(LEDC_LOW_SPEED_MODE, _channel, 0);
	}

	virtual void adjust(uint8_t level)
	{
		ledc_set_duty(LEDC_LOW_SPEED_MODE, _channel, level);
	}

protected:
	uint8_t _pin;
	ledc_channel_t _channel;
};

class BackLight : public PWMBase
{
public:
	BackLight(uint8_t pin, uint8_t channel = 0) : PWMBase(pin, channel)
	{
	}

	uint8_t getLevel()
	{
		return _level;
	}

	void adjust(uint8_t level)
	{
		_level = level;
		PWMBase::adjust(level);
		_on = true;
	}

	bool isOn()
	{
		return _on;
	};

	void on()
	{
		adjust(_level);
		_on = true;
	}

	void off()
	{
		adjust(0);
		_on = false;
	}

	bool reverse()
	{
		_on ? off() : on();
		return _on;
	}

private:
	bool _on{false};
	uint8_t _level{255};
};

// Huh? Nothing to do with tft

class PWMToneBase : public PWMBase
{
public:
	PWMToneBase(uint8_t pin, uint8_t channel, unsigned freq) : PWMBase(pin, channel), _freq(freq)
	{
	}

	virtual void begin()
	{
		PWMBase::begin();
	}

	virtual void onec(unsigned duration = 200)
	{
		ledc_set_freq(LEDC_LOW_SPEED_MODE, timerNum, _freq);
		_tick.initializeMs(duration, [this]() { ledc_stop(LEDC_LOW_SPEED_MODE, _channel, 0); });
		_tick.startOnce();
	}

protected:
	unsigned _freq;
	Timer _tick;
};

class Motor : public PWMToneBase
{
public:
	Motor(uint8_t pin, uint8_t channel = 1, unsigned freq = 1000) : PWMToneBase(pin, channel, freq)
	{
	}
};

class Buzzer : public PWMToneBase
{
public:
	Buzzer(uint8_t pin, uint8_t channel = 2, unsigned freq = 1000) : PWMToneBase(pin, channel, freq)
	{
	}
};

#endif
