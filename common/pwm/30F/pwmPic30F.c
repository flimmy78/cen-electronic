#include "../../../common/commons.h"

#include "../pwmPic.h"

#include <p30fxxxx.h>

void pwmInit() {
    // simple pwm mode without protection
    OC1CONbits.OCM = 0b110;
    OC2CONbits.OCM = 0b110;
    OC3CONbits.OCM = 0b110;
    OC4CONbits.OCM = 0b110;
}