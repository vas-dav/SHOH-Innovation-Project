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
	QueueHandle_t master_event_all_q = _qm->getQueue(ThreadCommon::QueueManager::master_event_all);
	ThreadCommon::Event data(ThreadCommon::Null, 0);
	int led = 0;
	bool LedState = true;
	for (;;) {
		xQueueReceive(master_event_all_q, static_cast<void*>(&data), portMAX_DELAY);
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
