/*
 * Master.cpp
 *
 *  Created on: 4 Apr 2023
 *      Author: tylen
 */

#include "Master.h"
#include "Log.h"
#include "ThreadCommon.h"
#include "Rotary.h"
#include "Manager.h"
#include "Logging.h"
#include "UserInterface.h"
#include "queue.h"
#include "Logging.h"

static const char* rotary_direction[] = 
{
	"Right",
	"Left",
	"Press",
	"Idle"
};

QueueHandle_t logging_queue;

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
	ThreadCommon::CommonManagers * manager = static_cast<ThreadCommon::CommonManagers*>(pvParams);

	manager->qm->createQueue(5,
							LOG_BUFFER_MAX_CAP,
							ThreadCommon::QueueManager::logging_message_all);
	logging_queue = manager->qm->getQueue(ThreadCommon::QueueManager::logging_message_all);
	manager->tm->createTask(thread_logging, "logging",
							configMINIMAL_STACK_SIZE * 10,tskIDLE_PRIORITY + 1UL,
							static_cast<void*>(manager));

	LOG_INFO("Logging Active");
	LOG_INFO("Started the real time kernel with preemption");
	LOG_INFO("Master Started");
	Master m(manager->qm);
	LOG_INFO("Master is creating queues");
	manager->qm->createQueue(100,
							sizeof(Event),
							ThreadCommon::QueueManager::master_event_all);
	manager->qm->createQueue(20,
							sizeof(Event),
							ThreadCommon::QueueManager::manager_event_master);
	manager->qm->createQueue(20,
							sizeof(UserInterface::InterfaceWithData),
							ThreadCommon::QueueManager::ui_event_manager);


	LOG_INFO("Master is creating tasks");
	manager->tm->createTask(thread_manager, "manager",
							configMINIMAL_STACK_SIZE * 10,tskIDLE_PRIORITY + 1UL,
							static_cast<void*>(manager));
	manager->tm->createTask(thread_rotary, "rotary",
							configMINIMAL_STACK_SIZE * 10,tskIDLE_PRIORITY + 1UL,
							static_cast<void*>(manager));
	manager->tm->createTask(thread_user_interface, "user_interface",
							configMINIMAL_STACK_SIZE * 10,tskIDLE_PRIORITY + 1UL,
							static_cast<void*>(manager));
	m.taskFunction();
}
