/*
 * SensorTempTC74.h
 *
 *  Created on: 16 May 2023
 */

#ifndef THREADS_TEMPERATURE_SENSORTEMPTC74_H_
#define THREADS_TEMPERATURE_SENSORTEMPTC74_H_

#include "I2C.h"

class SensorTempTC74 {
public:
	SensorTempTC74(I2C* pi2c);
	virtual ~SensorTempTC74();
	bool read();
private:
	I2C* _pi2c;
};

#endif /* THREADS_TEMPERATURE_SENSORTEMPTC74_H_ */
