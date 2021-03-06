#ifndef DUAL_H_BRIDGE_MOTOR
#define DUAL_H_BRIDGE_MOTOR

#include <stdbool.h>

#define HBRIDGE_1        1
#define HBRIDGE_2        2

// forward declaration
struct DualHBridgeMotor;
typedef struct DualHBridgeMotor DualHBridgeMotor;

/**
 * Type of function to initialize the DualHBridgeMotor.
 */
typedef bool dualHBridgeMotorInitFunction(DualHBridgeMotor* dualHBridgeMotor);

/**
* Read the value of the DualHBridge.
*/
typedef signed int dualHBridgeMotorReadValueFunction(DualHBridgeMotor* dualHBridgeMotor, unsigned int motorIndex);

/**
* Get the software revision of the dualHBrige Motor.
*/
typedef unsigned char dualHBridgeMotorGetSoftwareRevisionFunction(DualHBridgeMotor* dualHBridgeMotor);

/**
 * For a dual HBridge, manage 4 pwms (either 4 pwms in 30F, or 2 pwms and 2 pin directions on PIC32).
 * @param hBridgeSpeed1 between -255 and + 255
 * @param hBridgeSpeed2 between -255 and + 255
 */
typedef void dualHBridgeMotorWriteValueFunction(DualHBridgeMotor* dualHBridgeMotor, signed int hBridgeSpeed1, signed int hBridgeSpeed2);

/**
* Defines the contract for the Dual HBridge Motor.
*/
struct DualHBridgeMotor {
    /** The function which must be used to init the dual hBridge */
    dualHBridgeMotorInitFunction* dualHBridgeMotorInit;
    /** The function which must be used to read the value of each dualHBridge motor */
    dualHBridgeMotorReadValueFunction* dualHBridgeMotorReadValue;
    /** The function which can be used to change the value of each dualHBridge motor. */
    dualHBridgeMotorWriteValueFunction* dualHBridgeMotorWriteValue;
    /** The function which could be used to get the software revision. */
    dualHBridgeMotorGetSoftwareRevisionFunction* dualHBridgeMotorGetSoftwareRevision;
    /** To store the value of the motorSpeed1 (do not modify it without calling public methods) */
    signed int motorSpeed1;
    /** To store the value of the motorSpeed2 (do not modify it without calling public methods) */
    signed int motorSpeed2;
    /** pointer on other object (useful for I2C Connection for example) .*/
    int* object;
};

// PUBLIC FUNCTIONS

/** 
 * Init the wrapper around DualHBridge. 
 */
void initDualHBridge(DualHBridgeMotor* dualHBridgeMotors, 
               dualHBridgeMotorInitFunction* dualHBridgeMotorInit,
               dualHBridgeMotorReadValueFunction* dualHBridgeMotorReadValue,
               dualHBridgeMotorWriteValueFunction* dualHBridgeMotorWriteValue,
               dualHBridgeMotorGetSoftwareRevisionFunction* dualHBridgeMotorGetSoftwareRevision,
               int* object);

/** 
 * Stop the motor for the specified Dual HBridge Motors.
 */
void stopMotors(DualHBridgeMotor* dualHBridgeMotor);

/**
 * Convenient method to write the motor Speed
 * @param dualHBridgeMotor
 * @param hBridgeSpeed1
 * @param hBridgeSpeed2
 */
void setMotorSpeeds(DualHBridgeMotor* dualHBridgeMotor, signed int hBridgeSpeed1, signed int hBridgeSpeed2);


#endif
