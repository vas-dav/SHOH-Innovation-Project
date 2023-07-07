/*
 * ThreadCommon.h
 *
 *  Created on: 13 Apr 2023
 *      Author: tylen
 */

#ifndef __THREAD_COMMON_H_
#define __THREAD_COMMON_H_

#include "board.h"
#include "FreeRTOS.h"
#include <string>
#include <map>
#include "queue.h"
#include "pinportmap.h"
#include "task.h"
#include <assert.h>

namespace ThreadCommon
{
    enum RotaryAction 
	{
		Right,
		Left,
		Press,
		Idle
	};
    class ThreadManager
    {
        public:
            ThreadManager();
            ~ThreadManager() = default;
            bool createTask(void (*task_func)(void*), 
                            std::string name, 
                            size_t stack_size,
                            size_t priority,
                            void* parameters);
        private:
    };

    class QueueManager
    {
        public:
            enum Queue_id {
                master_event_all,
                relay_event_master,
                manager_event_master,
                ui_event_manager,
                logging_message_all
            };
            QueueManager();
            ~QueueManager() = default;
            bool createQueue(size_t queue_length, size_t item_size, Queue_id qid);
            QueueHandle_t getQueue(Queue_id qid);
            template<class T> bool send(Queue_id qid, T* data, TickType_t timeout){
            	QueueHandle_t q = this->getQueue(qid);
                BaseType_t qCheck = xQueueSend(q,
                                               static_cast<void*>(data),
                                               timeout);
                return (qCheck == pdTRUE);
            }
            template<class T> bool receive(Queue_id qid, T* data, TickType_t timeout){
            	QueueHandle_t q = this->getQueue(qid);
                BaseType_t qCheck = xQueueReceive(q,
                                                  static_cast<void*>(data),
                                                  timeout);
                return (qCheck == pdTRUE);
            }

        private:
            std::map <Queue_id, QueueHandle_t> queues;
    };

    typedef struct _CommonManagers
    {
        ThreadManager * tm;
        QueueManager * qm;
    } CommonManagers;
}

#endif /*__THREAD_COMMON_H_*/
