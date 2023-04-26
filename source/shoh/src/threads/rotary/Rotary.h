/*
 * Rotary.h
 *
 *  Created on: 26 Apr 2023
 *      Author: tylen
 */

#ifndef THREADS_ROTARY_ROTARY_H_
#define THREADS_ROTARY_ROTARY_H_

#include "Event.h"
#include "ThreadCommon.h"
#include "DigitalIoPin.h"

class Rotary {
public:
	Rotary(ThreadCommon::QueueManager* qm);
	virtual ~Rotary();
	void taskFunction();
private:
	Event* message;
	ThreadCommon::QueueManager* _qm;
	DigitalIoPin signal[3] = { { 0, 6, true, false, false, true, PIN_INT0_IRQn},
                               { 0, 5, true, false, false, true, PIN_INT1_IRQn},
                               { 1, 8, true, false, false, true, PIN_INT2_IRQn} };
};

void rotary_thread(void* pvParams);

#endif /* THREADS_ROTARY_ROTARY_H_ */
