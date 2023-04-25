#include "ThreadCommon.h"


ThreadCommon::ThreadManager::ThreadManager(){}

bool ThreadCommon::ThreadManager::createTask(void (*task_func)(void*),
                               std::string name, 
                               size_t stack_size,
                               size_t priority,
                               void* parameters)
{
    const char * t_name = name.c_str();
    BaseType_t taskCreated = xTaskCreate(task_func,
                                         t_name,
                                         stack_size,
                                         parameters,
                                         priority,
                                         NULL);
    assert(taskCreated == pdPASS);
    return (taskCreated == pdPASS);
}
