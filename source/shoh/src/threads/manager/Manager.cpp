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
	for(;;)
	{
		vTaskDelay(1);
	}
}

void thread_manager(void* pvParams)
{
	Manager m(static_cast<ThreadCommon::QueueManager*>(pvParams));
	m.taskFunction();
}