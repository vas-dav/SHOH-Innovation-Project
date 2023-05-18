/*
 * Temperature.h
 *
 *  Created on: 16 May 2023
 */

#ifndef THREADS_TEMPERATURE_TEMPERATURE_H_
#define THREADS_TEMPERATURE_TEMPERATURE_H_

#include "FreeRTOS.h"
#include "task.h"
#include "ThreadCommon.h"
#include "DigitalIoPin.h"
#include "I2C.h"

class Temperature {
public:
	Temperature(ThreadCommon::QueueManager* qm, I2C* pi2c);
	virtual ~Temperature();
	void taskFunction();
private:
	ThreadCommon::QueueManager* _qm;
	I2C* _pi2c;
};

void thread_temperature(void* pvParams);

#endif /* THREADS_TEMPERATURE_TEMPERATURE_H_ */
