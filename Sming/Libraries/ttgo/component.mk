COMPONENT_INCDIRS := src

TTGO_DRIVERS := \
	axp \
	bma423 \
	button \
	fx50xx \
	gps \
	i2c \
	mpu6050 \
	rtc \
	s7xg \
	tft

COMPONENT_SRCDIRS := $(addprefix src/ttgo/drive/,$(TTGO_DRIVERS))

