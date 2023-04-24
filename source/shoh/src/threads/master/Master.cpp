/*
 * Master.cpp
 *
 *  Created on: 4 Apr 2023
 *      Author: tylen
 */

#include "Master.h"

Master::Master(std::string name, size_t stack_size, void* pvParams, size_t task_priority)
: name(name), stack_size(stack_size), pvParams(pvParams), task_priority(task_priority)
{
	xTaskCreate (Master::taskWrapper, "Master_thread", this->stack_size, this->pvParams, this->task_priority, this->master_task_handle);	
}

Master::~Master(){}

void Master::taskFunction() {
	bool LedState = true;
	for (;;) {
		Board_LED_Set(led, LedState);
		LedState = (bool) !LedState;
		led++;
		if(led > 2){
			led = 0;
		}
		vTaskDelay(1000);
	}
}
