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
	DigitalIoPin signal[3] = { { 1, 18, true, true, false, true, PIN_INT0_IRQn}, //sigA
                               { 2, 12, true, true, false}, //sigB
                               { 0, 2, true, true, false, true, PIN_INT1_IRQn} }; //Press, sw //2 11 (pin without interrupt) //0 1 (board button)
};

void thread_rotary(void* pvParams);

#endif /* THREADS_ROTARY_ROTARY_H_ */
