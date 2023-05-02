/*
 * Manager.h
 *
 *  Created on: 29 Apr 2023
 *      Author: dave
 */

#ifndef THREADS_MANAGER_MANAGER_H_
#define THREADS_MANAGER_MANAGER_H_

#include "ThreadCommon.h"
#include "Event.h"
#include "Counter.h"
#include "Menu.h"


class Manager {
public:
	Manager(ThreadCommon::QueueManager* qm, Menu * menu);
	virtual ~Manager();
	void taskFunction();
private:
	Event::EventPair parseEvent(Event* e);
	ThreadCommon::QueueManager* _qm;
	Counter<EventRawData> set_point;
	Menu* _menu;

	EventRawData int_temp;
	EventRawData ext_temp;
	EventRawData rotary_action;
};

void thread_manager(void* pvParams);

#endif /* THREADS_MANAGER_MANAGER_H_ */
