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
#include "queue.h"
#include "task.h"
#include <assert.h>

namespace ThreadCommon
{
    typedef enum EventType
    {
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

    /* global variables */
    /* 'receiver'_'what'_'sender'_q */
    extern QueueHandle_t master_event_all_q;
    extern QueueHandle_t relay_event_master_q;
    extern QueueHandle_t manager_event_master_q;
    extern QueueHandle_t ui_event_manager_q;
}

#endif /*__THREAD_COMMON_H_*/
