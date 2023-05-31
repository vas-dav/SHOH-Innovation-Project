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
#include "Relay.h"
#include "Manager.h"
#include "Logging.h"
#include "UserInterface.h"
#include "Temperature.h"
#include "queue.h"
#include "esp8266_socket.h"

static const char* rotary_direction[] = 
{
	"Right",
	"Left",
	"Press",
	"Idle"
};

QueueHandle_t logging_queue;
Clock *global_clock = new Clock();

Master::Master(ThreadCommon::QueueManager* qm) : _qm(qm)
{
	LOG_DEBUG("Creating Master");
}

Master::~Master()
{
	LOG_ERROR("Master was deleted");
}

void Master::HandleEventType(Event* e)
{
	EventRawData rd = e->getData();
	bool send = false;
	switch (e->getType())
	{
		case Event::Null:
			LOG_ERROR("Master recieved Event::Null with data: %d", rd);
			break;
		case Event::Rotary:
			//Comes from rotary, goes to manager
			send = _qm->send<Event>(ThreadCommon::QueueManager::manager_event_master, e, 0);
			//LOG_WARNING("Timestamp: %zus, Clock: %zu, Chip freq: %zu", LPC_SCT1->COUNT_U / Chip_Clock_GetMainClockRate(), LPC_SCT1->COUNT_U, Chip_Clock_GetMainClockRate());
			if (send) LOG_DEBUG("Rotary: %s has been forwarded to manager", rotary_direction[rd]);
			break;
		case Event::InternalTemp:
			// TODO remove (deprecated)
			break;
		case Event::ExternalTemp:
			//Comes from sensors, goes to relay & manager
			send = _qm->send<Event>(ThreadCommon::QueueManager::relay_event_master, e, 0);
			send = _qm->send<Event>(ThreadCommon::QueueManager::manager_event_master, e, 0);
			if (send) LOG_DEBUG("ExtTemp: %d has been forwarded to manager and relay", rd);
			break;
		case Event::SetPoint:
			//Comes from manager, goes to relay 
			send = _qm->send<Event>(ThreadCommon::QueueManager::relay_event_master, e, 0);
			if (send) LOG_DEBUG("SetPoint: %d has been forwarded to relay", rd);
			break;
		default:
			LOG_ERROR("Unknown EventType");
			assert(0);
			break;
	}
}

void Master::taskFunction() {
	Event data(Event::Null, 0);
	//int soc = esp_socket("SSID", "PASSWORD");
	//int stat = esp_connect(soc, "IP", 5000);
	for (;;) 
	{
		if(!_qm->receive<Event>(ThreadCommon::QueueManager::master_event_all, &data, 10000))
		{
			data.setEvent(Event::Rotary, ThreadCommon::RotaryAction::Idle);
		}

		HandleEventType(&data);

		global_clock->updateClock();
		//LOG_WARNING("ESP socket status: %d", stat);
		//if(stat == 0)
		//	stat = esp_connect(soc, "IP", 5000);
	}
}


void thread_master(void* pvParams) {
	ThreadCommon::CommonManagers * manager = static_cast<ThreadCommon::CommonManagers*>(pvParams);

	manager->qm->createQueue(5,
							LOG_BUFFER_MAX_CAP,
							ThreadCommon::QueueManager::logging_message_all);
	logging_queue = manager->qm->getQueue(ThreadCommon::QueueManager::logging_message_all);
	manager->tm->createTask(thread_logging, "logging",
							configMINIMAL_STACK_SIZE * 9,tskIDLE_PRIORITY + 1UL,
							static_cast<void*>(manager));

	LOG_INFO("Logging Active");
	LOG_INFO("Started the real time kernel with preemption");
	LOG_INFO("Master Started");
	Master m(manager->qm);
	LOG_INFO("Master is creating queues");
	manager->qm->createQueue(60,
							sizeof(Event),
							ThreadCommon::QueueManager::master_event_all);
	manager->qm->createQueue(12,
							sizeof(Event),
							ThreadCommon::QueueManager::manager_event_master);
	manager->qm->createQueue(12,
							sizeof(UserInterface::InterfaceWithData),
							ThreadCommon::QueueManager::ui_event_manager);
	manager->qm->createQueue(5,
							sizeof(Event),
							ThreadCommon::QueueManager::relay_event_master);
	LOG_INFO("Master created queues");


	LOG_INFO("Master is creating tasks");
	manager->tm->createTask(thread_manager, "manager",
							configMINIMAL_STACK_SIZE * 15,tskIDLE_PRIORITY + 1UL,
							static_cast<void*>(manager));
	manager->tm->createTask(thread_rotary, "rotary",
							configMINIMAL_STACK_SIZE * 8,tskIDLE_PRIORITY + 1UL,
							static_cast<void*>(manager));
	manager->tm->createTask(thread_user_interface, "user_interface",
							configMINIMAL_STACK_SIZE * 8,tskIDLE_PRIORITY + 1UL,
							static_cast<void*>(manager));
	manager->tm->createTask(thread_relay, "relay",
							configMINIMAL_STACK_SIZE * 8,tskIDLE_PRIORITY + 1UL,
							static_cast<void*>(manager));
	manager->tm->createTask(thread_temperature, "temperature",
							configMINIMAL_STACK_SIZE * 8,tskIDLE_PRIORITY + 1UL,
							static_cast<void*>(manager));
	LOG_INFO("Master created tasks");
	m.taskFunction();
}
