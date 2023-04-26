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
				break;
			case ThreadCommon::RotaryAction::Left:
				Board_LED_Set(ThreadCommon::RotaryAction::Left, LedState);
				break;
			case ThreadCommon::RotaryAction::Press:
				Board_LED_Set(ThreadCommon::RotaryAction::Press, LedState);
				break;
			case ThreadCommon::RotaryAction::Idle:
				Board_LED_Set(ThreadCommon::RotaryAction::Right, LedState);
				break;
		}
		LedState = !LedState;
	}
}


void master_thread(void* pvParams) {
	Master m(static_cast<ThreadCommon::QueueManager*>(pvParams));
	m.taskFunction();
}
