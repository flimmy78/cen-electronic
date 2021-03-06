#include "eepromDevice.h"
#include "eepromDeviceInterface.h"

#include <stdbool.h>
#include <stdlib.h>

#include "../../common/error/error.h"

#include "../../common/io/printWriter.h"
#include "../../common/io/reader.h"

#include "../../common/log/logLevel.h"
#include "../../common/log/logger.h"
#include "../../common/log/logHandler.h"

#include "../../device/device.h"

static Eeprom* eeprom_;

void _deviceEepromCheckInitialized() {
    if (eeprom_ == NULL) {
        writeError(EEPROM_NULL);
    }
    if (!isEepromInitialized(eeprom_)) {
        writeError(EEPROM_NOT_INITIALIZED);
    }
}

void deviceEepromInit(void) {
    _deviceEepromCheckInitialized();
}

void deviceEepromShutDown(void) {
}

bool isEepromDeviceOk(void) {
    // TODO : Check by reading a value in the EEPROM
    return true;
}

void deviceEepromHandleRawData(char commandHeader, InputStream* inputStream, OutputStream* outputStream, OutputStream* notificationOutputStream) {
    _deviceEepromCheckInitialized();
    if (commandHeader == COMMAND_RELOAD_EEPROM) {
        ackCommand(outputStream, EEPROM_DEVICE_HEADER, COMMAND_RELOAD_EEPROM);
        eeprom_->eepromLoad(eeprom_);
    }
    else if (commandHeader == COMMAND_DUMP_TO_FILE_EEPROM) {
        ackCommand(outputStream, EEPROM_DEVICE_HEADER, COMMAND_DUMP_TO_FILE_EEPROM);
        eeprom_->eepromDump(eeprom_);
    }
    else if (commandHeader == COMMAND_DUMP_TO_LOG_OUTPUT_STREAM_EEPROM) {
        ackCommand(outputStream, EEPROM_DEVICE_HEADER, COMMAND_DUMP_TO_LOG_OUTPUT_STREAM_EEPROM);
        OutputStream* debugOutputStream = getInfoOutputStreamLogger();
        dumpEepromToOutputStream(eeprom_, debugOutputStream, 0, eeprom_->maxIndex);
    }
	else if (commandHeader == COMMAND_DUMP_PARTIAL_CONTENT_TO_LOG_OUTPUT_STREAM_EEPROM) {
		ackCommand(outputStream, EEPROM_DEVICE_HEADER, COMMAND_DUMP_PARTIAL_CONTENT_TO_LOG_OUTPUT_STREAM_EEPROM);
		unsigned long startAddress = readHex4(inputStream);
		checkIsSeparator(inputStream);
		unsigned long length = readHex4(inputStream);
		OutputStream* debugOutputStream = getInfoOutputStreamLogger();
		dumpEepromToOutputStream(eeprom_, debugOutputStream, startAddress, startAddress + length);
	}
    else if (commandHeader == COMMAND_FORMAT_EEPROM) {
        ackCommand(outputStream, EEPROM_DEVICE_HEADER, COMMAND_FORMAT_EEPROM);
        unsigned long startAddress = readHex4(inputStream);
        checkIsSeparator(inputStream);
        unsigned long length = readHex4(inputStream);
        checkIsSeparator(inputStream);
        unsigned char clearValue = readHex2(inputStream);
        clearEeprom(eeprom_, startAddress, length, clearValue);
    }
    else if (commandHeader == COMMAND_READ_BYTE_EEPROM) {
        ackCommand(outputStream, EEPROM_DEVICE_HEADER, COMMAND_READ_BYTE_EEPROM);
        unsigned long address = readHex4(inputStream);
        char value = eeprom_->eepromReadChar(eeprom_, address);
        appendHex2(outputStream, value);
    }
    else if (commandHeader == COMMAND_READ_INT_EEPROM) {
        ackCommand(outputStream, EEPROM_DEVICE_HEADER, COMMAND_READ_INT_EEPROM);
        unsigned long address = readHex4(inputStream);
        int value = eepromReadInt(eeprom_, address);
        appendHex4(outputStream, value);
    }
    else if (commandHeader == COMMAND_WRITE_BYTE_EEPROM) {
        ackCommand(outputStream, EEPROM_DEVICE_HEADER, COMMAND_WRITE_BYTE_EEPROM);
        unsigned long address = readHex4(inputStream);
        checkIsSeparator(inputStream);
        char data = readHex2(inputStream);
        eeprom_->eepromWriteChar(eeprom_, address, data);
    }
    else if (commandHeader == COMMAND_WRITE_INT_EEPROM) {
        ackCommand(outputStream, EEPROM_DEVICE_HEADER, COMMAND_WRITE_INT_EEPROM);
        unsigned long address = readHex4(inputStream);
        checkIsSeparator(inputStream);
        int data = readHex4(inputStream);
        eepromWriteInt(eeprom_, address, data);
    }
    else if (commandHeader == COMMAND_READ_BLOCK_EEPROM) {
        ackCommand(outputStream, EEPROM_DEVICE_HEADER, COMMAND_READ_BLOCK_EEPROM);
        unsigned long address = readHex4(inputStream);
        int index;
        for (index = 0; index < EEPROM_DEVICE_READ_BLOCK_LENGTH; index++) {
            char value = eeprom_->eepromReadChar(eeprom_, address + index);
            if (index > 0) {
                appendSeparator(outputStream);
            }
            appendHex2(outputStream, value);
        }
    }
    else if (commandHeader == COMMAND_WRITE_BLOCK_EEPROM) {
        ackCommand(outputStream, EEPROM_DEVICE_HEADER, COMMAND_WRITE_BLOCK_EEPROM);
        unsigned long address = readHex4(inputStream);
        char data;
        int index;
        for (index = 0; index < EEPROM_DEVICE_WRITE_BLOCK_LENGTH; index++) {
            checkIsSeparator(inputStream);
            data = readHex2(inputStream);
            eeprom_->eepromWriteChar(eeprom_, address + index, data);
        }
    }
    else if (commandHeader == COMMAND_INTENSIVE_TEST) {
        ackCommand(outputStream, EEPROM_DEVICE_HEADER, COMMAND_INTENSIVE_TEST);
        unsigned long address = readHex4(inputStream);
        checkIsSeparator(inputStream);
        unsigned long length = readHex4(inputStream);
        unsigned int errorCount = 0;
        unsigned int index;
        // Writes
        for (index = 0; index < length; index++) {
            unsigned char value = (unsigned char) index;
            eeprom_->eepromWriteChar(eeprom_, address + index, value);
        }
        // Reads
        for (index = 0; index < length; index++) {
            unsigned char value = (unsigned char) eeprom_->eepromReadChar(eeprom_, address + index);
            if (value != (unsigned char)index) {
                errorCount++;
            }
        }
        appendHex4(outputStream, errorCount);
    }
}

static DeviceDescriptor descriptor = {
    .deviceInit = &deviceEepromInit,
    .deviceShutDown = &deviceEepromShutDown,
    .deviceIsOk = &isEepromDeviceOk,
    .deviceHandleRawData = &deviceEepromHandleRawData,
};

DeviceDescriptor* getEepromDeviceDescriptor(Eeprom* eepromParam) {
    eeprom_ = eepromParam;
    return &descriptor;
}
