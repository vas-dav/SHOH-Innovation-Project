/*
 * Logging.cpp
 *
 *  Created on: 12 May 2023
 *      Author: tylen
 */

#include "Logging.h"
#include "ThreadCommon.h"
#include "Log.h"
#include "queue.h"

Logging::Logging(ThreadCommon::QueueManager * qm) : _qm(qm) {
	q = _qm->getQueue(ThreadCommon::QueueManager::logging_message_all);
}

Logging::~Logging() {
	// TODO Auto-generated destructor stub
}

void Logging::taskFunction()
{
	for(;;)
	{
		char data[LOG_BUFFER_MAX_CAP] = {0};
		xQueueReceive(q, static_cast<char*>(data), portMAX_DELAY);
		mutex.lock();
		printf("%s\n", data);
		mutex.unlock();
	}
}

void thread_logging(void* pvParams)
{
	ThreadCommon::CommonManagers * manager = static_cast<ThreadCommon::CommonManagers*>(pvParams);
	Logging log(manager->qm);
	log.taskFunction();
}
