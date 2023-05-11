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

void Master::HandleEventType(Event* e, Event::EventType type)
{
	switch (type) 
	{
		case Event::Null:
			break;
		case Event::Rotary:
			//Comes from rotary, goes to manager
			_qm->send<Event>(ThreadCommon::QueueManager::manager_event_master, e, 0);
			DebugRotaryEvent(e->getDataOf(Event::Rotary));
			break;
		case Event::InternalTemp:
			// TODO remove (deprecated)
			break;
		case Event::ExternalTemp:
			//Comes from sensors, goes to relay & manager
			_qm->send<Event>(ThreadCommon::QueueManager::relay_event_master, e, 0);
			_qm->send<Event>(ThreadCommon::QueueManager::manager_event_master, e, 0);
			break;
		case Event::SetPoint:
			//TODO comes from manager, goes to relay 
			break;
		default:
			assert(0);
			break;
	}
}

void Master::DebugRotaryEvent(EventRawData e_data)
{
	bool LedState = true;
	switch(e_data)
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
				//Board_LED_Set(ThreadCommon::RotaryAction::Right, LedState);
				printf("Idle\r\n");
				break;
		}
		LedState = !LedState;
}

void Master::taskFunction() {
	Event data(Event::Null, 0);
	for (;;) {
		if(!_qm->receive<Event>(ThreadCommon::QueueManager::master_event_all, &data, 10000))
			data.setDataOf(Event::Rotary, ThreadCommon::RotaryAction::Idle);
		
		for(Event::EventType i :
	    {Event::Null, 
		 Event::Rotary, 
		 Event::InternalTemp,
		 Event::ExternalTemp,
		 Event::SetPoint})
		 {
			if (data.getDataOf(i) != ERROR_RETURN)
			{
				HandleEventType(&data, i);
			}
		 }	
	}
}


void thread_master(void* pvParams) {
	Master m(static_cast<ThreadCommon::QueueManager*>(pvParams));
	m.taskFunction();
}
