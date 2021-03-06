#include <stdbool.h>
#include <stdlib.h>

#include "servoPwm.h"
#include "../pwmPic.h"

#include "../../../common/delay/cenDelay.h"

#include "../../../common/error/error.h"

#include "../../../common/io/outputStream.h"
#include "../../../common/io/printWriter.h"

#include "../../../common/log/logger.h"
#include "../../../common/log/logLevel.h"

#include "../../../common/timer/cenTimer.h"
#include "../../../common/timer/timerConstants.h"
#include "../../../common/timer/timerList.h"

#define SERVO_SPEED_TIMER_FACTOR 3

/**
 * The servos.
 */
static ServoList servoList;

/**
 * Private.
 */
Servo* getServo(int index) {
    return &(servoList.servos[index]);
}

/**
 * Only for debug.
 */
ServoList* _getServoList() {
    return &servoList;
}

/**
 * The interrupt timer.
 */
void interruptServoTimerCallbackFunc(Timer* timer) {
    if (!servoList.useTimer) {
        return;
    }
    int i;
    for (i = 0; i < PWM_COUNT; i++) {
        Servo* servo = getServo(i);
        if (servo->currentPosition == servo->targetPosition) {
            continue;
        }
        if (servo->currentPosition < servo->targetPosition) {
            servo->currentPosition += servo->speed * SERVO_SPEED_TIMER_FACTOR;
            if (servo->currentPosition > servo->targetPosition) {
                servo->currentPosition = servo->targetPosition;
            }
        } else if (servo->currentPosition > servo->targetPosition) {
            servo->currentPosition -= servo->speed * SERVO_SPEED_TIMER_FACTOR;
            // Limit
            if (servo->currentPosition < servo->targetPosition) {
                servo->currentPosition = servo->targetPosition;
            }
        }
        __internalPwmServo(i + 1, servo->currentPosition);
    }
}

// PUBLIC INTERFACCE

void initPwmForServo(unsigned int servoToActivateMask, int posInit) {
    if (servoList.initialized) {
        return;
    }
    __internalPwmForServoHardware(servoToActivateMask, posInit);
    // Init servo structure
    int i;
    for (i = 0; i < PWM_COUNT; i++) {
        Servo* servo = getServo(i);
        servo->enabled = servoToActivateMask & (1 << i);
        servo->speed = PWM_SERVO_SPEED_MAX;
        servo->currentPosition = posInit;
        servo->targetPosition = posInit;
    }

    // Init the timer for servo
    // and add the timer to the list, so that the interrupt function will
    // update at a certain frequency the position of the servo
    addTimer(SERVO_TIMER_CODE,
                            TIME_DIVIDER_50_HERTZ,
                            &interruptServoTimerCallbackFunc,
                            "SERVO", 
							NULL);
    servoList.initialized = true;
    servoList.useTimer = true;
}

/**
 * Check if the servoIndex is ok, if it's not, raise an error.
 * @param servoIndex between 1 and n, if <= 0 or > n, raise an error
 * @param errorString the error string if there is a problem
 * @return true if servoIndex is ok, false else
 */
bool checkServoIndex(int servoIndex, char* errorString) {
    if (servoIndex == 0 || servoIndex > PWM_COUNT) {
        writeError(ILLEGAL_ARGUMENT_EXCEPTION);
        appendString(getErrorOutputStreamLogger(), errorString);
        return false;
    }
    return true;
}

// PUBLIC WRITE FUNCTIONS

void pwmServo(int servoIndex, unsigned int speed, int targetPosition) {
    if (!checkServoIndex(servoIndex, "=> pwmServo")) {
        return;
    }
    Servo* servo = getServo(servoIndex - 1);
    if (!servo->enabled) {
        return;
    }
    servo->speed = speed;
    servo->targetPosition = targetPosition;
    // By default, we update the value immediately, if we want some speed, we need a timer !
    if (!servoList.useTimer) {
        __internalPwmServo(servoIndex, (targetPosition));
    }
}

void pwmServoAll(unsigned int speed, int dutyms) {
    int i;
    for (i = 1; i <= PWM_COUNT; i++) {
        pwmServo(i, speed, dutyms);
    }
}

// READ FUNCTIONS

unsigned int pwmServoReadSpeed(int servoIndex) {
    if (!checkServoIndex(servoIndex, "=> pwmServoReadSpeed")) {
        return -1;
    }
    Servo* servo = getServo(servoIndex - 1);
    return servo->speed;
}

unsigned int pwmServoReadCurrentPosition(int servoIndex) {
    if (!checkServoIndex(servoIndex, "=> pwmServoReadCurrentPosition")) {
        return -1;
    }
    Servo* servo = getServo(servoIndex - 1);
    return servo->currentPosition;
}

unsigned int pwmServoReadTargetPosition(int servoIndex) {
    if (!checkServoIndex(servoIndex, "=> pwmServoReadTargetPosition")) {
        return -1;
    }
    Servo* servo = getServo(servoIndex - 1);
    return servo->targetPosition;
}
