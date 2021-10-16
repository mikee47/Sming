/*
  Button2.cpp - Arduino Library to simplify working with buttons.
  Created by Lennart Hennigs, October 28, 2017.
*/
#pragma once

#include <cstdint>

#define DEBOUNCE_MS 50
#define LONGCLICK_MS 2500
#define DOUBLECLICK_MS 400

enum ClickType {
	NO_CLICK = 0,
	SINGLE_CLICK = 1,
	DOUBLE_CLICK = 2,
	TRIPLE_CLICK = 3,
	LONG_CLICK = 4,
};

class Button2
{
public:
	using CallbackFunction = void (*)();

	Button2()
	{
		pin = 0xFF;
	}

	Button2(uint8_t attachTo, uint8_t buttonMode = INPUT_PULLUP, unsigned int debounceTimeout = DEBOUNCE_MS);

	void setDebounceTime(unsigned int ms)
	{
		debounce_time_ms = ms;
	}

	void setChangedHandler(CallbackFunction f)
	{
		change_cb = f;
	}

	void setPressedHandler(CallbackFunction f)
	{
		pressed_cb = f;
	}

	void setReleasedHandler(CallbackFunction f)
	{
		released_cb = f;
	}

	void setClickHandler(CallbackFunction f)
	{
		click_cb = f;
	}

	void setTapHandler(CallbackFunction f)
	{
		tap_cb = f;
	}

	void setLongClickHandler(CallbackFunction f)
	{
		long_cb = f;
	}

	void setDoubleClickHandler(CallbackFunction f)
	{
		double_cb = f;
	}

	void setTripleClickHandler(CallbackFunction f)
	{
		triple_cb = f;
	}

	unsigned int wasPressedFor()
	{
		return down_time_ms;
	}

	bool isPressed()
	{
		return state == LOW;
	}

	unsigned int getNumberOfClicks()
	{
		return click_count;
	}

	unsigned int getClickType()
	{
		return last_click_type;
	}

	uint8_t getAttachPin()
	{
		return pin;
	}

	bool operator==(Button2& rhs)
	{
		return this == &rhs;
	}

	void loop();

private:
	uint8_t pin;
	int prev_state;
	int state{HIGH};
	uint8_t click_count{0};
	ClickType last_click_type{NO_CLICK};
	unsigned long click_ms;
	unsigned long down_ms;
	unsigned int debounce_time_ms;
	unsigned int down_time_ms{0};
	bool pressed_triggered{false};
	bool longclick_detected{false};

	CallbackFunction pressed_cb{nullptr};
	CallbackFunction released_cb{nullptr};
	CallbackFunction change_cb{nullptr};
	CallbackFunction tap_cb{nullptr};
	CallbackFunction click_cb{nullptr};
	CallbackFunction long_cb{nullptr};
	CallbackFunction double_cb{nullptr};
	CallbackFunction triple_cb{nullptr};
};
