/*
 * Master.cpp
 *
 *  Created on: 4 Apr 2023
 *      Author: tylen
 */

#include "Master.h"

void Master::taskFunction() {
	int led = 0;
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


void master_thread(void* pvParams) {
	Master m;
	m.taskFunction();
}