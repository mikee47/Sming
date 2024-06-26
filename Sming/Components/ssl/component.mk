# => SSL
COMPONENT_SRCDIRS	:= src
COMPONENT_INCDIRS	:= include

CONFIG_VARS				+= ENABLE_SSL
COMPONENT_RELINK_VARS	:= ENABLE_SSL
ENABLE_SSL				?= 0
ifeq ($(ENABLE_SSL),$(filter $(ENABLE_SSL),1 Axtls))
	override ENABLE_SSL	:= Axtls
	COMPONENT_DEPENDS	:= axtls-8266
	COMPONENT_SRCDIRS	+= Axtls
	APP_CFLAGS			+= -DENABLE_SSL
else ifeq ($(ENABLE_SSL),Bearssl)
	override ENABLE_SSL	:= Bearssl
	COMPONENT_DEPENDS	:= bearssl-esp8266
	COMPONENT_SRCDIRS	+= BearSsl
	APP_CFLAGS			+= -DENABLE_SSL
else ifeq ($(ENABLE_SSL),Mbedtls)
$(error MBED TLS not yet implemented)
else ifeq ($(ENABLE_SSL),0)
	COMPONENT_SRCDIRS	+= NoSsl
else
$(error Unsupported ENABLE_SSL value "$(ENABLE_SSL)". Supported values are Axtls, Bearssl or 0 to disable SSL)
endif

#
COMPONENT_DEPENDS		+= crypto

COMPONENT_RELINK_VARS	+= SSL_DEBUG
SSL_DEBUG				?= 0
ifeq ($(SSL_DEBUG),1)
	COMPONENT_CXXFLAGS	+= -DSSL_DEBUG=1
endif

# Prints SSL status when App gets built
CUSTOM_TARGETS			+= check-ssl
.PHONY:check-ssl
check-ssl:
ifeq ($(ENABLE_SSL),0)
	$(info SSL support disabled)
else
	$(info Using $(ENABLE_SSL) SSL implementation)

##@Tools

.PHONY: generate-cert
generate-cert: include/ssl/private_key.h ##Generate X509 certificate and private key files in include/ssl

SSL_TOOLS_PATH			:= $(COMPONENT_PATH)/Tools
SSL_INCLUDE_DIR			:= $(PROJECT_DIR)/include/ssl
OUT_SSL                 := out/ssl/
SSL_CERT_DIR			:= $(OUT_SSL)/cert

include/ssl/private_key.h:
	$(info Generating unique certificate and key. This may take some time...)
	$(Q) mkdir -p $(SSL_INCLUDE_DIR) $(SSL_CERT_DIR)
	$(Q) chmod a+x $(SSL_TOOLS_PATH)/make_certs.sh
	$(Q) cd $(SSL_CERT_DIR); SSL_INCLUDE_DIR=$(SSL_INCLUDE_DIR) $(SSL_TOOLS_PATH)/make_certs.sh

endif

COMPONENT_DOXYGEN_INPUT := \
	include

COMPONENT_DOXYGEN_PREDEFINED := \
	ENABLE_SSL=1

