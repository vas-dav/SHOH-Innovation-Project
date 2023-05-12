/*
 * Relay.h
 *
 *  Created on: 12 May 2023
 *      Author: tylen
 */

#ifndef THREADS_RELAY_RELAY_H_
#define THREADS_RELAY_RELAY_H_

#include "ThreadCommon.h"

class Relay {
public:
	Relay(ThreadCommon::QueueManager* qm);
	virtual ~Relay();
	void taskFunction();
private:
	ThreadCommon::QueueManager* _qm;
};

void thread_relay(void * pvParams);

#endif /* THREADS_RELAY_RELAY_H_ */
