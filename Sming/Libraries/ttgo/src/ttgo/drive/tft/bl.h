#pragma once

#include "driver/ledc.h"
#include <SimpleTimer.h>

class PWMBase
{
public:
	static constexpr ledc_mode_t mode{LEDC_LOW_SPEED_MODE};
	static constexpr ledc_timer_t timerNum{LEDC_TIMER_0};

	PWMBase(uint8_t pin, uint8_t channel) : _pin(pin), _channel(channel)
	{
	}

	virtual ~PWMBase();
	virtual void begin();
	virtual void adjust(uint8_t level);

protected:
	uint8_t _pin;
	uint8_t _channel;
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
		PWMBase::adjust(level);
		_level = level;
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

	virtual void onec(unsigned duration = 200);

protected:
	unsigned _freq;
	SimpleTimer _tick;
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
