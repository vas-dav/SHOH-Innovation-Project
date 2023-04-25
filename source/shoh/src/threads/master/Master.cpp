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
		Board_LED_Set(led, LedState);
		LedState = (bool) !LedState;
		led++;
		if(led > 2){
			led = 0;
		}
		vTaskDelay(1000);
	}
}


void master_thread(void* pvParams) {
	Master m(static_cast<ThreadCommon::QueueManager*>(pvParams));
	m.taskFunction();
}
