/*
 * Event.h
 *
 *  Created on: 26 Apr 2023
 *      Author: tylen
 */

#ifndef THREADS_COMMON_EVENT_H_
#define THREADS_COMMON_EVENT_H_

typedef short int EventRawData;

class Event
{
public:
    enum EventType
        {
            Null,
            Rotary,
            InternalTemp,
            ExternalTemp,
            SetPoint,
            NotifyUI
        };


    Event(Event::EventType type, EventRawData data)
    {
        setEvent(type, data);
    } 

    void setEvent(Event::EventType type, EventRawData data)
    {
        _type = type;
        _data = data;
    }

    Event::EventType inline getType() const
    {
        return _type;
    }

    EventRawData inline  getData() const
    {
        return _data;
    }

private:
    Event::EventType _type;
    EventRawData _data;
};


#endif /* THREADS_COMMON_EVENT_H_ */
