COMPONENT_RELINK_VARS += ENABLE_MALLOC_COUNT
COMPONENT_DOXYGEN_INPUT := include

# Not currently supported on MacOS
ifeq ($(UNAME),Darwin)
override ENABLE_MALLOC_COUNT=0
else
ENABLE_MALLOC_COUNT ?= 1
endif

ifeq ($(ENABLE_MALLOC_COUNT),1)

COMPONENT_CXXFLAGS += -DENABLE_MALLOC_COUNT=1

# Hook all the memory allocation functions we need to monitor heap activity
MC_WRAP_FUNCS := \
	malloc \
	calloc \
	realloc \
	free \
	strdup
MC_DEFSYMS := \
	__wrap_malloc=mc_malloc \
	__wrap_calloc=mc_calloc \
	__wrap_realloc=mc_realloc \
	__wrap_free=mc_free

ifeq ($(SMING_ARCH),Esp8266)
MC_WRAP_FUNCS += \
	realloc \
	pvPortMalloc \
	pvPortCalloc \
	pvPortRealloc \
	pvPortZalloc \
	pvPortZallocIram \
	vPortFree
MC_DEFSYMS += \
	__wrap_pvPortMalloc=mc_malloc \
	__wrap_pvPortCalloc=mc_calloc \
	__wrap_pvPortRealloc=mc_realloc \
	__wrap_pvPortZalloc=mc_zalloc \
	__wrap_pvPortZallocIram=mc_zalloc \
	__wrap_vPortFree=mc_free
endif

EXTRA_LDFLAGS := \
	$(call UndefWrap,$(MC_WRAP_FUNCS)) \
	$(call DefSym,$(MC_DEFSYMS))

endif
