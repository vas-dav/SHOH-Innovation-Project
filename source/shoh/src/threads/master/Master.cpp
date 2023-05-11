/*
 * Master.cpp
 *
 *  Created on: 4 Apr 2023
 *      Author: tylen
 */

#include "Master.h"
#include "Log.h"

static const char* rotary_direction[] = 
{
	"Right",
	"Left",
	"Press",
	"Idle"
};

Master::Master(ThreadCommon::QueueManager* qm) : _qm(qm)
{
	LOG_DEBUG("Creating Master");
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
			LOG_DEBUG("Rotary: %s has been forwarded to manager", rotary_direction[e->getDataOf(type)]);
			break;
		case Event::InternalTemp:
			// TODO remove (deprecated)
			break;
		case Event::ExternalTemp:
			//Comes from sensors, goes to relay & manager
			_qm->send<Event>(ThreadCommon::QueueManager::relay_event_master, e, 0);
			_qm->send<Event>(ThreadCommon::QueueManager::manager_event_master, e, 0);
			LOG_DEBUG("ExtTemp: %d has been forwarded to manager and relay", e->getDataOf(type));
			break;
		case Event::SetPoint:
			//Comes from manager, goes to relay 
			_qm->send<Event>(ThreadCommon::QueueManager::relay_event_master, e, 0);
			LOG_DEBUG("SetPoint: %d has been forwarded to relay", e->getDataOf(type));
			break;
		default:
			assert(0);
			break;
	}
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
