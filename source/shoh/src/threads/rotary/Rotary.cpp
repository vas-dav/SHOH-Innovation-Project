/*
 * Rotary.cpp
 *
 *  Created on: 26 Apr 2023
 *      Author: tylen
 */

#include "Rotary.h"

Rotary::Rotary(ThreadCommon::QueueManager* qm) : _qm(qm) {}

Rotary::~Rotary() {}

void Rotary::taskFunction()
{
Event data(Event::Null, 0);
	Event* e = new Event(Event::Rotary, ThreadCommon::RotaryAction::Idle);
	_qm->send<Event>(ThreadCommon::QueueManager::master_event_all, e, 10);
	for (;;) {}
}

void rotary_thread(void* pvParams)
{
	Rotary r(static_cast<ThreadCommon::QueueManager*>(pvParams));
	r.taskFunction();
}
