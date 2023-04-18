/*
 * ThreadCommon.h
 *
 *  Created on: 13 Apr 2023
 *      Author: tylen
 */
#include "board.h"

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
}
