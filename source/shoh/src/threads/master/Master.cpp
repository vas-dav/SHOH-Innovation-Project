/*
 * Master.cpp
 *
 *  Created on: 4 Apr 2023
 *      Author: tylen
 */

#include "Master.h"

Master::Master(ThreadCommon::QueueManager* qm) : _qm(qm)
{

}

void Master::taskFunction() {
	ThreadCommon::Event data(ThreadCommon::Null, 0);
	int led = 0;
	bool LedState = true;
	for (;;) {
		_qm->receive<ThreadCommon::Event>(ThreadCommon::QueueManager::master_event_all, &data, portMAX_DELAY);
		if(data.getData() == 1 && data.getType() == ThreadCommon::EventType::Rotary){
			Board_LED_Set(led, LedState);
		}
	}
}


void master_thread(void* pvParams) {
	Master m(static_cast<ThreadCommon::QueueManager*>(pvParams));
	m.taskFunction();
}
