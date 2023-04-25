/*
 * QueueManager.cpp
 *
 *  Created on: 24 Apr 2023
 *      Author: tylen
 */

#include "ThreadCommon.h"

ThreadCommon::QueueManager::QueueManager() {}

bool ThreadCommon::QueueManager::createQueue(size_t queue_length, size_t item_size, Queue_id qid)
{
    QueueHandle_t queue_to_create;
    if ((queue_to_create = xQueueCreate(queue_length, item_size)))
    {
        queues.insert({qid, queue_to_create});
        return true;
    }
    return false;
}

QueueHandle_t ThreadCommon::QueueManager::getQueue(Queue_id qid)
{
    const auto pos = queues.find(qid);
    if (pos == queues.end())
        return nullptr;
    
    return pos->second;
    
   //return queues.at(qid);
}
