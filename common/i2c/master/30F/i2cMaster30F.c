#include "../../../../common/commons.h"

#include <i2c.h>

#include "../i2cMaster.h"
#include "../../../../common/i2c/i2cCommon.h"


inline int portableMasterWriteI2C(char data) {
	return MasterWriteI2C(data);
}

inline char portableMasterReadI2C() {
	return MasterReadI2C();
}

inline void portableCloseI2C() {
	CloseI2C();
}

inline void portableMasterWaitSendI2C( void ) {
	while (I2CCONbits.SEN) {
	
	}
}