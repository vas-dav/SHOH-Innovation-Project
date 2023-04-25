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
#include "task.h"
#include <assert.h>

namespace ThreadCommon
{
    typedef enum EventType
    {
        Null,
        Rotary,
        Temperature,
        Manager
    };

    class Event
    {
    public:
        Event(ThreadCommon::EventType type, uint8_t data)
        {
            _type = type;
            _data = data;
        }

        ThreadCommon::EventType getType() const
        {
            return _type;
        }

        uint8_t getData() const
        {
            return _data;
        }

    private:
        ThreadCommon::EventType _type;
        uint8_t _data;
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
                ui_event_manager
            };
            QueueManager();
            ~QueueManager() = default;
            bool createQueue(size_t queue_length, size_t item_size, Queue_id qid);
            QueueHandle_t getQueue(Queue_id qid);
        private:
            std::map <Queue_id, QueueHandle_t> queues;
    };

    /* global variables */
    /* 'receiver'_'what'_'sender'_q */
    /*
    extern QueueHandle_t master_event_all_q;
    extern QueueHandle_t relay_event_master_q;
    extern QueueHandle_t manager_event_master_q;
    extern QueueHandle_t ui_event_manager_q;
    */
}

#endif /*__THREAD_COMMON_H_*/
