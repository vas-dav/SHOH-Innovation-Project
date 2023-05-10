/*
 * Manager.cpp
 *
 *  Created on: 29 Apr 2023
 *      Author: dave
 */

#include "Manager.h"
#include "ThreadCommon.h"

Manager::Manager(ThreadCommon::QueueManager* qm)
: _qm(qm), _menu{qm}
{
}

Manager::~Manager() 
{
	// TODO Auto-generated destructor stub
}

Event::EventPair Manager::parseEvent(Event* e)
{
	EventRawData raw_data;
	for(Event::EventType i :
	    {Event::Rotary, Event::InternalTemp,
		 Event::ExternalTemp})
	{
		raw_data = e->getDataOf(i);
		if(raw_data != ERROR_RETURN)
		{
			Event::EventPair p = {raw_data, i};
			return p; 
		}
	}
	return {ERROR_RETURN, Event::Null};
}

void Manager::taskFunction()
{
	Event data(Event::Null, 0);
	Event::EventPair event_pair = {0, Event::EventType::Null};
	for(;;)
	{
		_qm->receive<Event>(ThreadCommon::QueueManager::manager_event_master, &data, portMAX_DELAY);
		event_pair= this->parseEvent(&data);
        _menu.HandleEventPair(&event_pair);
	}
}

void thread_manager(void* pvParams)
{
	Manager m(static_cast<ThreadCommon::QueueManager*>(pvParams));
	m.taskFunction();
}
