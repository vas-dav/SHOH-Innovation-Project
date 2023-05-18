/*
 * Manager.cpp
 *
 *  Created on: 29 Apr 2023
 *      Author: dave
 */

#include "Manager.h"
#include "ThreadCommon.h"
#include "Log.h"

Manager::Manager(ThreadCommon::QueueManager* qm)
: _qm(qm), _menu{qm}
{
	LOG_DEBUG("Creating Manager");
}

Manager::~Manager() 
{
	LOG_ERROR("Deleting Manager");
}

void Manager::taskFunction()
{
	Event data(Event::Null, 0);
	for(;;)
	{
		_qm->receive<Event>(ThreadCommon::QueueManager::manager_event_master, &data, portMAX_DELAY);
        _menu.parseEvent(&data);
	}
}

void thread_manager(void* pvParams)
{
	ThreadCommon::CommonManagers * manager = static_cast<ThreadCommon::CommonManagers*>(pvParams);
	Manager m(manager->qm);
	m.taskFunction();
}
