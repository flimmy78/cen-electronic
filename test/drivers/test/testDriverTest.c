#include "testDriverTest.h"

#include <stdlib.h>

#include "../../../test/unity/unity.h"
#include "../../../common/clock/clock.h"
#include "../../../common/error/error.h"
#include "../../../common/io/stream.h"
#include "../../../common/io/pc/consoleOutputStream.h"
#include "../../../common/log/logger.h"
#include "../../../common/log/logLevel.h"
#include "../../../common/log/pc/consoleLogHandler.h"

#include "../../../device/device.h"
#include "../../../device/deviceList.h"

#include "../../../device/clock/clockDevice.h"
#include "../../../device/clock/clockDeviceInterface.h"

#include "../../../device/test/testDevice.h"
#include "../../../device/test/testDeviceInterface.h"

#include "../../../device/motor/pwmMotorDevice.h"
#include "../../../device/motor/pwmMotorDeviceInterface.h"


#include "../../../drivers/clock/mockClock.h"
#include "../../../drivers/driverList.h"
#include "../../../drivers/dispatcher/driverDataDispatcherList.h"
#include "../../../drivers/dispatcher/localDriverDataDispatcher.h"
#include "../../../drivers/test/testDriver.h"

#include "../../../remote/clock/remoteClock.h"


// Dispatchers
#define TEST_DRIVER_TEST_DATA_DISPATCHER_LIST_LENGTH 2
static DriverDataDispatcher driverDataDispatcherListArray[TEST_DRIVER_TEST_DATA_DISPATCHER_LIST_LENGTH];

// Drivers
#define TEST_DRIVER_TEST_REQUEST_DRIVER_BUFFER_LENGTH	40
static Buffer driverRequestBuffer;
static char driverRequestBufferArray[TEST_DRIVER_TEST_REQUEST_DRIVER_BUFFER_LENGTH];
#define TEST_DRIVER_TEST_RESPONSE_DRIVER_BUFFER_LENGTH	40
static Buffer driverResponseBuffer;
static char driverResponseBufferArray[TEST_DRIVER_TEST_RESPONSE_DRIVER_BUFFER_LENGTH];


// Devices
#define TEST_DRIVER_TEST_DEVICE_LIST_LENGTH		10
static Device deviceListArray[TEST_DRIVER_TEST_DEVICE_LIST_LENGTH];

void testDriverTestTestSuite(void) {
	RUN_TEST(test_testDriverGetValue);
}

void test_testDriverGetValue(void) {
	initLog(DEBUG);
	addConsoleLogHandler(DEBUG);

	// Dispatchers
	initDriverDataDispatcherList((DriverDataDispatcher(*)[]) &driverDataDispatcherListArray, TEST_DRIVER_TEST_DATA_DISPATCHER_LIST_LENGTH);
	addLocalDriverDataDispatcher();

	// Init the drivers
	initDrivers(&driverRequestBuffer, (char(*)[]) &driverRequestBufferArray, TEST_DRIVER_TEST_REQUEST_DRIVER_BUFFER_LENGTH,
		&driverResponseBuffer, (char(*)[]) &driverResponseBufferArray, TEST_DRIVER_TEST_RESPONSE_DRIVER_BUFFER_LENGTH);

	// Get test driver for debug purpose
	addDriver(testDriverGetDescriptor(), TRANSMIT_LOCAL);

	// Devices
	initDeviceList((Device(*)[]) &deviceListArray, TEST_DRIVER_TEST_DEVICE_LIST_LENGTH);
	addLocalDevice(getTestDeviceInterface(), getTestDeviceDescriptor());

	// Test Driver Test

	int actual = testDriverGetValue(10, 20);
	TEST_ASSERT_EQUAL(30, actual);

	// Clock Driver Test

	Clock clock;
	initMockClock(&clock);
	addLocalDevice(getClockDeviceInterface(), getClockDeviceDescriptor(&clock));

	ClockData clockData;
	getRemoteClockData(&clockData);
	TEST_ASSERT_EQUAL(14, clockData.year);

	// Motor Driver Test

}