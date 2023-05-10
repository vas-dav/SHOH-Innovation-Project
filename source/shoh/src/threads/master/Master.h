/*
 * Master.h
 *
 *  Created on: 4 Apr 2023
 *      Author: tylen
 */

#ifndef THREADS_MASTER_MASTER_H_
#define THREADS_MASTER_MASTER_H_

#include "chip.h"
#include "board.h"
#include "ThreadCommon.h"
#include "Event.h"
#include "task.h"
#include <string>

class Master {
public:
	Master(ThreadCommon::QueueManager* qm);
	virtual ~Master() = default;
	void taskFunction();

private:
	Event* message;
	ThreadCommon::QueueManager* _qm;
};

void thread_master(void* pvParams);


#endif /* THREADS_MASTER_MASTER_H_ */
