COMPONENT_SRCDIRS := src
COMPONENT_INCDIRS := include
COMPONENT_DOXYGEN_INPUT := include
COMPONENT_DEPENDS := simpleRPC

# Architecture of the device where the hosted service will be flashed
HOSTED_ARCH ?= Esp8266

COMPONENT_VARS := ENABLE_HOSTED
ENABLE_HOSTED ?=

ifneq ($(ENABLE_HOSTED),)
	COMPONENT_SRCDIRS += init/$(ENABLE_HOSTED) 
	EXTRA_LDFLAGS := $(call Wrap,_Z4initv)
	COMPONENT_DEPENDS += SerialLib
	ifeq ($(ENABLE_HOSTED),tcp)
		COMPONENT_DEPENDS += Network
		DISABLE_NETWORK := 0 
		DISABLE_WIFI := 0
	endif
endif

COMPONENT_RELINK_VARS += \
	HOSTED_SERVER_IP \
	HOSTED_COM_PORT \
	HOSTED_COM_SPEED

HOSTED_COM_PORT ?= $(COM_PORT)
HOSTED_COM_SPEED ?= 115200

COMPONENT_CFLAGS = \
	-DHOSTED_SERVER_IP=$(HOSTED_SERVER_IP) \
	-DHOSTED_COM_PORT="\"$(HOSTED_COM_PORT)"\" \
	-DHOSTED_COM_SPEED=$(HOSTED_COM_SPEED)

COMPONENT_CXXFLAGS := $(COMPONENT_CFLAGS)
