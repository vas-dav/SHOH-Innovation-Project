/*
 * Manager.h
 *
 *  Created on: 29 Apr 2023
 *      Author: dave
 */

#ifndef THREADS_MANAGER_MANAGER_H_
#define THREADS_MANAGER_MANAGER_H_

#include "ThreadCommon.h"
#include "Event.h"

class Manager {
public:
	Manager(ThreadCommon::QueueManager* qm);
	virtual ~Manager();
	void taskFunction();
private:
	Event* message;
	ThreadCommon::QueueManager* _qm;
};

void thread_manager(void* pvParams);

#endif /* THREADS_MANAGER_MANAGER_H_ */
