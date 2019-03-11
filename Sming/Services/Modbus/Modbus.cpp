/*
 *
 * Modbus uses UART0 via MAX485 chip with extra GPIO to control direction.
 *
 *  - Transactions will easily fit into hardware FIFO so no need for additional memory overhead;
 *  - ISR set to trigger on completion of transmit/receive packets rather than individual bytes;
 *
 */

#include "Modbus.h"
#include "Digital.h"
#include "HardwareSerial.h"
#include "Platform/System.h"

#if DEBUG_VERBOSE_LEVEL == DBG
#define MODBUS_DEBUG
#endif

String modbusExceptionString(modbus_exception_t status)
{
	switch(status) {
	case MBE_Success:
		return F("Success");
	case MBE_IllegalFunction:
		return F("IllegalFunction");
	case MBE_IllegalDataAddress:
		return F("IllegalDataAddress");
	case MBE_IllegalDataValue:
		return F("MBES_IllegalDataValue");
	case MBE_SlaveDeviceFailure:
		return F("SlaveDeviceFailure");
	case MBE_InvalidSlaveID:
		return F("InvalidSlaveID");
	case MBE_InvalidFunction:
		return F("InvalidFunction");
	case MBE_ResponseTimedOut:
		return F("ResponseTimedOut");
	case MBE_InvalidCRC:
		return F("InvalidCRC");
	default:
		return F("Unknown") + String(status);
	}
}

static uint16_t crc16_update(uint16_t crc, uint8_t a)
{
	crc ^= a;

	for(unsigned i = 0; i < 8; ++i) {
		if(crc & 1)
			crc = (crc >> 1) ^ 0xA001;
		else
			crc = (crc >> 1);
	}

	return crc;
}

void Modbus::uartCallback(uart_t* uart, uint32_t status)
{
	auto modbus = reinterpret_cast<Modbus*>(uart_get_callback_param(uart));
	// Guard against spurious interrupts
	if(modbus == nullptr) {
		return;
	}

	// Tx FIFO empty
	if(status & _BV(UIFE)) {
		// Set MAX485 back into read mode
		digitalWrite(modbus->txEnablePin, 0);
	}

	// Rx FIFO full or timeout
	if(status & (_BV(UIFF) | _BV(UITO))) {
		modbus->receiveComplete();
	}
}

// Prepare for communication with a slave device
void Modbus::begin(uint8_t txEnablePin, ModbusCallback callback)
{
	this->txEnablePin = txEnablePin;
	transactionCallback = callback;

	Serial.end();
	Serial.setUartCallback(nullptr);
	Serial.setTxBufferSize(0);
	Serial.setRxBufferSize(0);
	Serial.begin(MODBUS_DEFAULT_BAUDRATE);

	digitalWrite(txEnablePin, 0);
	pinMode(txEnablePin, OUTPUT);
}

/*
 * Submit a modbus command.
 *
 * The ADU must be valid and contain two reserved bytes at the end for the checksum,
 * which this method calculates.
 *
 * We write data into the hardware FIFO: no need for any software buffering.
 *
 */
bool Modbus::execute(ModbusTransaction& mbt)
{
	// Fail if transaction already in progress
	if(transaction != nullptr) {
		return false;
	}

	if(mbt.baudrate == 0) {
		debug_w("Using default baudrate for request");
		mbt.baudrate = MODBUS_DEFAULT_BAUDRATE;
	}

#ifdef MODBUS_DEBUG
	printf_P(_F("> %02X %02X"), mbt.slaveId, mbt.function);
	for(uint8_t i = 0; i < mbt.dataSize; ++i) {
		printf_P(_F(" %02X"), mbt.data[i]);
	}
#endif

	// Prepare UART for comms
	Serial.setBaudRate(mbt.baudrate);
	Serial.clear();

	// Transmit request data whilst keeping TX_EN assserted
	digitalWrite(txEnablePin, 1);

	// Address, function & data
	uint16_t crc = 0xFFFF;

	// Write value and update CRC
	auto write = [&](uint8_t c) {
		Serial.write(c);
		crc = crc16_update(crc, c);
	};

	write(mbt.slaveId);
	write(mbt.function);
	for(uint8_t i = 0; i < mbt.dataSize; ++i) {
		write(mbt.data[i]);
	}
	Serial.write(lowByte(crc));
	Serial.write(highByte(crc));

	// Frame-end - tx interrupt occurs when last byte is removed from buffer so cannot be real data
	Serial.write((uint8_t)0);

	// Now prepare for response
	mbt.status = MBE_ResponseTimedOut;
	mbt.dataSize = 0;
	transaction = &mbt;
	Serial.setUartCallback(uartCallback, this);

#ifdef MODBUS_DEBUG
	printf_P(_F(" %02X %02X\n"), lowByte(crc), highByte(crc));
#endif

	// Put a timeout on the overall transaction
	timer.setCallback(
		[](void* arg) {
			Serial.setUartCallback(nullptr);
			reinterpret_cast<Modbus*>(arg)->completeTransaction(MBE_ResponseTimedOut);
		},
		this);
	timer.startMs(MODBUS_TRANSACTION_TIMEOUT_MS);

	return true;
}

/*
 * Called from ISR on receive timeout, so we should have complete response in FIFO.
 *
 * Check/validate response, invoke callback.
 */
void Modbus::receiveComplete()
{
	Serial.setUartCallback(nullptr);
	timer.stop();

	// Now need to schedule a task callback to process the received data
	System.queueCallback(
		[](uint32_t param) {
			auto modbus = reinterpret_cast<Modbus*>(param);
			auto status = modbus->processResponse();
			modbus->completeTransaction(status);
		},
		reinterpret_cast<uint32_t>(this));
}

modbus_exception_t Modbus::processResponse()
{
	auto size = (size_t)Serial.available();
	// SlaveID, Function, CRC with at least 1 data byte
	if(size < 5) {
		return MBE_ResponseTimedOut; // Not really the right exception code...
	}

	uint16_t crc = 0xFFFF;

	// Read byte from serial receive FIFO and update CRC
	auto read = [&]() {
		auto c = (uint8_t)Serial.read();
		crc = crc16_update(crc, c);
		return c;
	};

	uint8_t slaveId = read();
	uint8_t function = read();

	size -= 4; // data size
	if(size > MODBUS_DATA_SIZE) {
#ifdef MODBUS_DEBUG
		debug_e("Too much data - %u in FIFO", size + 4);
#endif
		return MBE_InvalidCRC; // It will fail CRC anyway so quit now
	}

	for(uint8_t i = 0; i < size; ++i) {
		transaction->data[i] = read();
	}
	transaction->dataSize = size;
	uint8_t crc1 = read();
	uint8_t crc2 = read();

#ifdef MODBUS_DEBUG
	printf_P(_F("< %02X %02X"), slaveId, function);
	for(unsigned i = 0; i < transaction->dataSize; ++i) {
		printf_P(" %02X", transaction->data[i]);
	}
	printf_P(_F(" %02X %02X\n"), crc1, crc2);
#endif

	// verify response is for correct Modbus slave
	if(slaveId != transaction->slaveId) {
		return MBE_InvalidSlaveID;
	}

	// verify response is for correct Modbus function code (mask exception bit 7)
	if((function & 0x7F) != transaction->function) {
		return MBE_InvalidFunction;
	}

	// check whether Modbus exception occurred; return Modbus Exception Code
	if(function & 0x80) {
		return modbus_exception_t(transaction->data[0]);
	}

	if(crc != 0) {
		return MBE_InvalidCRC;
	}

	// OK
	return MBE_Success;
}

/*
 * Called by serial ISR when transaction has completed, or by expiry timer.
 */
void Modbus::completeTransaction(modbus_exception_t status)
{
	debug_i("Modbus::completeTransaction(): %s", modbusExceptionString(status).c_str());

	ModbusTransaction* mbt = transaction;
	transaction = nullptr;

	mbt->status = status;
	if(transactionCallback) {
		transactionCallback(*mbt);
	}
}
