/*
 * Master.h
 *
 *  Created on: 4 Apr 2023
 *      Author: tylen
 */

#ifndef THREADS_MASTER_MASTER_H_
#define THREADS_MASTER_MASTER_H_

#include "chip.h"
#include "board.h"
#include "FreeRTOSCPP/Task.hpp"
#include "FreeRTOSCPP/Kernel.hpp"
#include "task.h"


class Master : public FreeRTOS::Task {
public:
	Master(): FreeRTOS::Task((tskIDLE_PRIORITY + 1UL),
			  configMINIMAL_STACK_SIZE * 10,
			  "master"){};
	virtual ~Master() = default;

	Master(Master&&) noexcept = default;
	Master& operator=(Master&&) noexcept = default;

	void taskFunction();
private:
	int led;
};


#endif /* THREADS_MASTER_MASTER_H_ */
