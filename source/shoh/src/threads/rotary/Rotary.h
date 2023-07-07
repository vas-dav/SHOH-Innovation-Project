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
	DigitalIoPin signal[3] = { { ThreadCommon::PORT_ROTARY_SIG_A, 
								 ThreadCommon::PIN_ROTARY_SIG_A, 
								 true, true, false, true, PIN_INT0_IRQn}, 
                               { ThreadCommon::PORT_ROTARY_SIG_B, 
							     ThreadCommon::PIN_ROTARY_SIG_B,
								 true, true, false}, 
                               { ThreadCommon::PORT_ROTARY_PRESS, 
							     ThreadCommon::PIN_ROTARY_PRESS,
								 true, true, false, true, PIN_INT1_IRQn} };
};

void thread_rotary(void* pvParams);

#endif /* THREADS_ROTARY_ROTARY_H_ */
