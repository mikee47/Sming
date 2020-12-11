
$MAKE_PARALLEL Basic_Blink Basic_DateTime Basic_Delegates Basic_Interrupts Basic_ProgMem Basic_Serial Basic_Servo Basic_Ssl LiveDebug DEBUG_VERBOSE_LEVEL=3

# Build and run tests
export SMING_TARGET_OPTIONS='--flashfile=$(FLASH_BIN) --flashsize=$(SPI_SIZE)'
$MAKE_PARALLEL tests

# Build the documentation
mv $SMING_PROJECTS_DIR/samples ..
mv $SMING_PROJECTS_DIR/tests ..
unset SMING_PROJECTS_DIR
make docs V=1
