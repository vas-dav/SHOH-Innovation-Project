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

class Rotary {
public:
	Rotary(ThreadCommon::QueueManager* qm);
	virtual ~Rotary();
	void taskFunction();
private:
	Event* message;
	ThreadCommon::QueueManager* _qm;
};

void rotary_thread(void* pvParams);

#endif /* THREADS_ROTARY_ROTARY_H_ */
