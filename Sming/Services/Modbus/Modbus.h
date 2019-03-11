#ifndef __MODBUS_H
#define __MODBUS_H

#include <user_config.h>
#include "Delegate.h"
#include "SimpleTimer.h"
#include "WString.h"
#include "espinc/uart.h"

#define MODBUS_DEFAULT_BAUDRATE 9600

#define MODBUS_TRANSACTION_TIMEOUT_MS 300 ///< Response must be received within this time

// Maximum number of bytes in transaction data
#ifndef MODBUS_DATA_SIZE
#define MODBUS_DATA_SIZE 64
#endif

// Modbus exception codes
enum __attribute__((packed)) modbus_exception_t {
	/**
	 Modbus protocol illegal function exception.

	 The function code received in the query is not an allowable action for
	 the server (or slave). This may be because the function code is only
	 applicable to newer devices, and was not implemented in the unit
	 selected. It could also indicate that the server (or slave) is in the
	 wrong state to process a request of this type, for example because it is
	 unconfigured and is being asked to return register values.

	 @ingroup constant
	 */
	MBE_IllegalFunction = 0x01,

	/**
	 Modbus protocol illegal data address exception.

	 The data address received in the query is not an allowable address for
	 the server (or slave). More specifically, the combination of reference
	 number and transfer length is invalid. For a controller with 100
	 registers, the ADU addresses the first register as 0, and the last one
	 as 99. If a request is submitted with a starting register address of 96
	 and a quantity of registers of 4, then this request will successfully
	 operate (address-wise at least) on registers 96, 97, 98, 99. If a
	 request is submitted with a starting register address of 96 and a
	 quantity of registers of 5, then this request will fail with Exception
	 Code 0x02 "Illegal Data Address" since it attempts to operate on
	 registers 96, 97, 98, 99 and 100, and there is no register with address
	 100.

	 @ingroup constant
	 */
	MBE_IllegalDataAddress = 0x02,

	/**
	 Modbus protocol illegal data value exception.

	 A value contained in the query data field is not an allowable value for
	 server (or slave). This indicates a fault in the structure of the
	 remainder of a complex request, such as that the implied length is
	 incorrect. It specifically does NOT mean that a data item submitted for
	 storage in a register has a value outside the expectation of the
	 application program, since the MODBUS protocol is unaware of the
	 significance of any particular value of any particular register.

	 @ingroup constant
	 */
	MBE_IllegalDataValue = 0x03,

	/**
	 Modbus protocol slave device failure exception.

	 An unrecoverable error occurred while the server (or slave) was
	 attempting to perform the requested action.

	 @ingroup constant
	 */
	MBE_SlaveDeviceFailure = 0x04,

	// Class-defined success/exception codes
	/**
	 ModbusMaster success.

	 Modbus transaction was successful, the following checks were valid:
	 - slave ID
	 - function code
	 - response code
	 - data
	 - CRC

	 @ingroup constant
	 */
	MBE_Success = 0x00,

	/**
	 ModbusMaster invalid response slave ID exception.

	 The slave ID in the response does not match that of the request.

	 @ingroup constant
	 */
	MBE_InvalidSlaveID = 0xE0,

	/**
	 ModbusMaster invalid response function exception.

	 The function code in the response does not match that of the request.

	 @ingroup constant
	 */
	MBE_InvalidFunction = 0xE1,

	/**
	 ModbusMaster response timed out exception.

	 The entire response was not received within the timeout period,
	 ModbusMaster::MB_ResponseTimeout.

	 @ingroup constant
	 */
	MBE_ResponseTimedOut = 0xE2,

	/**
	 ModbusMaster invalid response CRC exception.

	 The CRC in the response does not match the one calculated.

	 @ingroup constant
	 */
	MBE_InvalidCRC = 0xE3,
};

// Modbus function codes
enum __attribute__((packed)) modbus_function_t {
	MB_None = 0x00,
	// Bit-wise access
	MB_ReadCoils = 0x01,
	MB_ReadDiscreteInputs = 0x02,
	MB_WriteSingleCoil = 0x05,
	MB_WriteMultipleCoils = 0x0F,

	// 16-bit access
	MB_ReadHoldingRegisters = 0x03,
	MB_ReadInputRegisters = 0x04,
	MB_WriteSingleRegister = 0x06,
	MB_WriteMultipleRegisters = 0x10,
	MB_MaskWriteRegister = 0x16,
	MB_ReadWriteMultipleRegisters = 0x17,
};

String modbusExceptionString(modbus_exception_t status);

// 16-bit words are stored big-endian; use this macro to pull them from response buffer
static inline uint16_t modbus_makeword(const uint8_t* buffer)
{
	return (buffer[0] << 8) | buffer[1];
}

/** @brief Public details of a transaction */
struct ModbusTransaction {
	// The callback parameter
	uint32_t param;
	// These values are provided by the caller
	uint8_t slaveId;
	unsigned baudrate;
	/* These values are set by calls to CModbus */
	modbus_function_t function;
	/* These values are returned to the caller via callback */
	modbus_exception_t status;
	// Buffer for input/response data
	uint8_t data[MODBUS_DATA_SIZE];
	uint8_t dataSize;
};

/** @brief  Callback type for modbus transaction */
typedef std::function<void(ModbusTransaction& mbt)> ModbusCallback;

class Modbus
{
public:
	Modbus() : transaction(nullptr)
	{
	}

	void begin(uint8_t txEnablePin, ModbusCallback callback);

	bool busy() const
	{
		return transaction != nullptr;
	}

	bool execute(ModbusTransaction& mbt);

private:
	static void IRAM_ATTR uartCallback(uart_t* uart, uint32_t status);
	void IRAM_ATTR receiveComplete();
	modbus_exception_t processResponse();
	void completeTransaction(modbus_exception_t status);

private:
	uint8_t txEnablePin = 0;
	// Current transaction, nullptr if we're not doing anything
	ModbusTransaction* transaction;
	// Completion callback
	ModbusCallback transactionCallback;
	// Use to schedule callback and timeout
	SimpleTimer timer;
};

#endif
