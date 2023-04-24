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
#include "threads/common/ThreadCommon.h"
#include "task.h"
#include <string>

class Master {
public:
	Master(std::string name, size_t stack_size, void* pvParams, size_t task_priority);
	virtual ~Master();
	void taskFunction();

	static void taskWrapper(void *pvParams) {
		Master* pSelf = reinterpret_cast<Master*>(pvParams);
		pSelf->taskFunction();
	}

	//Master(Master&&) noexcept = default;
	//Master& operator=(Master&&) noexcept = default;
private:
	int led;
	std::string name;
	size_t stack_size;
	void* pvParams;
	size_t task_priority;
	TaskHandle_t* master_task_handle;
	//std::shared_ptr<ThreadCommon::Event> message;
};


#endif /* THREADS_MASTER_MASTER_H_ */
