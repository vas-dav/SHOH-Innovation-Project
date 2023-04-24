/*
 * ThreadCommon.h
 *
 *  Created on: 13 Apr 2023
 *      Author: tylen
 */
#include "board.h"
#include "FreeRTOS.h"
#include "queue.h"

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

    /* global variables */
    /* 'receiver'_'what'_'sender'_q */
    extern QueueHandle_t master_event_all_q;
    extern QueueHandle_t relay_event_master_q;
    extern QueueHandle_t manager_event_master_q;
    extern QueueHandle_t ui_event_manager_q;

    
}
