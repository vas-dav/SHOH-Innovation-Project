/*
 * Relay.cpp
 *
 *  Created on: 12 May 2023
 *      Author: tylen
 */

#include "Relay.h"
#include "Event.h"
#include "Log.h"

Relay::Relay(ThreadCommon::QueueManager* qm): _qm(qm) {
	LOG_DEBUG("Creating Relay");
}

Relay::~Relay() {
	LOG_ERROR("Deleting Relay");
}

void Relay::taskFunction()
{
	Event data(Event::Null, 0);
	for(;;)
	{
		_qm->receive<Event>(ThreadCommon::QueueManager::relay_event_master, &data, portMAX_DELAY);
	}
}

void thread_relay(void * pvParams)
{
	ThreadCommon::CommonManagers * manager = static_cast<ThreadCommon::CommonManagers*>(pvParams);
	Relay r(manager->qm);
}