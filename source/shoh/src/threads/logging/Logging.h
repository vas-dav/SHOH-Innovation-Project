/*
 * Logging.h
 *
 *  Created on: 12 May 2023
 *      Author: tylen
 */

#ifndef THREADS_LOGGING_LOGGING_H_
#define THREADS_LOGGING_LOGGING_H_

#include "Fmutex.h"
#include "ThreadCommon.h"

class Logging {
public:
	Logging(ThreadCommon::QueueManager * qm);
	virtual ~Logging();
	void taskFunction();
private:
	ThreadCommon::QueueManager * _qm;
	QueueHandle_t q;
	Fmutex mutex;
};

void thread_logging(void* pvParams);

#endif /* THREADS_LOGGING_LOGGING_H_ */
