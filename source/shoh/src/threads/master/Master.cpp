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
	bool LedState = true;
	for (;;) {
		_qm->receive<Event>(ThreadCommon::QueueManager::master_event_all, &data, portMAX_DELAY);
		switch(data.getDataOf(Event::Rotary))
		{
			case ThreadCommon::RotaryAction::Right:
				Board_LED_Set(ThreadCommon::RotaryAction::Right, LedState);
				printf("Right\r\n");
				break;
			case ThreadCommon::RotaryAction::Left:
				Board_LED_Set(ThreadCommon::RotaryAction::Left, LedState);
				printf("Left\r\n");
				break;
			case ThreadCommon::RotaryAction::Press:
				Board_LED_Set(ThreadCommon::RotaryAction::Press, LedState);
				printf("Press\r\n");
				break;
			case ThreadCommon::RotaryAction::Idle:
				Board_LED_Set(ThreadCommon::RotaryAction::Right, LedState);
				printf("Idle\r\n");
				break;
		}
		LedState = !LedState;
	}
}


void thread_master(void* pvParams) {
	Master m(static_cast<ThreadCommon::QueueManager*>(pvParams));
	m.taskFunction();
}
