#include "ThreadCommon.h"
#include "Log.h"


ThreadCommon::ThreadManager::ThreadManager(){}

bool ThreadCommon::ThreadManager::createTask(void (*task_func)(void*),
                               std::string name, 
                               size_t stack_size,
                               size_t priority,
                               void* parameters)
{
    const char * t_name = name.c_str();
    LOG_DEBUG("Creating task [name: %s, priority: %ld, stack: %ld]", 
             t_name, priority, stack_size);
    BaseType_t taskCreated = xTaskCreate(task_func,
                                         t_name,
                                         stack_size,
                                         parameters,
                                         priority,
                                         NULL);
    if (!(taskCreated == pdPASS)) 
    {
        LOG_ERROR("Failed to create a task [name: %s, priority: %ld, stack: %ld]", 
                  t_name, priority, stack_size)
    }
    return (taskCreated == pdPASS);
}
