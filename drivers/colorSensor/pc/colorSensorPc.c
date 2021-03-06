#include "colorSensorPc.h"

#include <stdbool.h>
#include <stdlib.h>
#include "../../../common/color/color.h"

/**
 * ColorSensor Implementation (POO)
 */
bool colorSensorPcInit(ColorSensor* colorSensor) {
    return true;
}

/**
* ColorSensor Implementation (POO)
*/
Color* colorSensorPcReadValue(ColorSensor* colorSensor) {
    return colorSensor->color;
}

void initColorSensorPc(ColorSensor* colorSensor, 
                       Color* pColor,
                       colorSensorFindColorTypeFunction* colorSensorFindColorType) {
    initColorSensor(colorSensor, 
                    &colorSensorPcInit,
                    &colorSensorPcReadValue,
                    colorSensorFindColorType,
                    pColor,
                    NULL);
}
