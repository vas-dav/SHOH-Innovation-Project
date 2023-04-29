/*
 * Manager.cpp
 *
 *  Created on: 29 Apr 2023
 *      Author: dave
 */

#include "Manager.h"
#include "ThreadCommon.h"

Manager::Manager(ThreadCommon::QueueManager* qm) : _qm(qm)
{
	// TODO Auto-generated constructor stub

}

Manager::~Manager() 
{
	// TODO Auto-generated destructor stub
}

void Manager::taskFunction()
{
	Event data(Event::Null, 0);
	for(;;)
	{
		_qm->receive<Event>(ThreadCommon::QueueManager::manager_event_master, &data, portMAX_DELAY);
	}
}

void thread_manager(void* pvParams)
{
	Manager m(static_cast<ThreadCommon::QueueManager*>(pvParams));
	m.taskFunction();
}