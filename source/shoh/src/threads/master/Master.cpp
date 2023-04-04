/*
 * Master.cpp
 *
 *  Created on: 4 Apr 2023
 *      Author: tylen
 */

#include "Master.h"

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

