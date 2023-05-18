/*
 * Event.h
 *
 *  Created on: 26 Apr 2023
 *      Author: tylen
 */

#ifndef THREADS_COMMON_EVENT_H_
#define THREADS_COMMON_EVENT_H_

#include <string>
#include <map>

typedef short int EventRawData;
const EventRawData ERROR_RETURN = -999; // Soon to be depercated

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

    typedef struct _EventPair 
    {
        EventRawData rd;
        EventType et;
    } EventPair; // Soon to be depercated

    Event(Event::EventType type, EventRawData data) // Soon to be depercated
    {
        events.insert({type, data});
    }

    void inline addData(Event::EventType type, EventRawData data) // Soon to be depercated
    {
        const auto pos = events.find(type);
        // No duplicates
        if (pos == events.end())
            events.insert({type, data});
    }

    EventRawData getDataOf(Event::EventType e) const // Soon to be depercated
    {
        const auto pos = events.find(e);
        if (pos == events.end())
            return ERROR_RETURN;
        return pos->second;
    }

    void inline setDataOf(Event::EventType e, EventRawData data) // Soon to be depercated
    {
        events[e] = data;
    }

/*    Event(Event::EventType type, EventRawData data)
    {
        setEvent(type, data);
    } */

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
    std::map <Event::EventType, EventRawData> events;
    Event::EventType _type;
    EventRawData _data;
};


#endif /* THREADS_COMMON_EVENT_H_ */
