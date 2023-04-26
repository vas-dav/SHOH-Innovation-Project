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
	Event data(Event::Null, 0);
	int led = 0;
	bool LedState = true;
	for (;;) {
		_qm->receive<Event>(ThreadCommon::QueueManager::master_event_all, &data, portMAX_DELAY);
		if(data.getDataOf(Event::Rotary) == ThreadCommon::RotaryAction::Idle){
			Board_LED_Set(led, LedState);
		}
	}
}


void master_thread(void* pvParams) {
	Master m(static_cast<ThreadCommon::QueueManager*>(pvParams));
	m.taskFunction();
}
