COMPONENT_SRCDIRS := src
COMPONENT_INCDIRS := include
COMPONENT_DOXYGEN_INPUT := include

CRYPTO_DEFSYMS := \
	crypto_sha224_update=crypto_sha256_update \
	crypto_sha224_get_state=crypto_sha256_get_state \
	crypto_sha224_set_state=crypto_sha256_set_state \
	crypto_sha512_update=crypto_sha384_update \
	crypto_sha512_get_state=crypto_sha384_get_state \
	crypto_sha512_set_state=crypto_sha384_set_state \

EXTRA_LDFLAGS := \
	$(call DefSym,$(CRYPTO_DEFSYMS))
