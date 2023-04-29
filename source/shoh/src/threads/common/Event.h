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
const EventRawData ERROR_RETURN = -999;

class Event
{
public:
    enum EventType
        {
            Null,
            Rotary,
            InternalTemp,
            ExternalTemp,
            SetPoint
        };

    typedef struct _EventPair 
    {
        EventRawData rd;
        EventType et;
    } EventPair;

    Event(Event::EventType type, EventRawData data)
    {
        events.insert({type, data});
    }

    void inline addData(Event::EventType type, EventRawData data)
    {
        const auto pos = events.find(type);
        // No duplicates
        if (pos == events.end())
            events.insert({type, data});
    }

    EventRawData getDataOf(Event::EventType e) const
    {
        const auto pos = events.find(e);
        if (pos == events.end())
            return ERROR_RETURN;
        return pos->second;
    }

    void inline setDataOf(Event::EventType e, EventRawData data)
    {
        events[e] = data;
    }

private:
    std::map <Event::EventType, EventRawData> events;
};


#endif /* THREADS_COMMON_EVENT_H_ */
