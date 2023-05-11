#include "chip.h"
#include "board.h"
#include "FreeRTOS.h"
#include "task.h"
#include <cr_section_macros.h>
#include "retarget_uart.h"

#include "ThreadCommon.h"
#include "Master.h"
#include "Rotary.h"
#include "Manager.h"
#include "UserInterface.h"
#include "Log.h"


int main(void)
{
  LOG_INFO("Starting system init");
  SystemCoreClockUpdate();
  Board_Init();

  retarget_init();

  LOG_INFO("System init done");

  ThreadCommon::ThreadManager* manager = new ThreadCommon::ThreadManager;
  ThreadCommon::QueueManager* qmanager = new ThreadCommon::QueueManager;
  LOG_INFO("Creating queues");
  qmanager->createQueue(100,
                        sizeof(Event),
                        ThreadCommon::QueueManager::master_event_all);
  qmanager->createQueue(20,
                        sizeof(Event),
                        ThreadCommon::QueueManager::manager_event_master);
  qmanager->createQueue(20,
                        sizeof(UserInterface::InterfaceWithData),
                        ThreadCommon::QueueManager::ui_event_manager);

  LOG_INFO("Creating tasks");
  manager->createTask(thread_master, "master",
                      configMINIMAL_STACK_SIZE * 10,tskIDLE_PRIORITY + 1UL,
                      static_cast<void*>(qmanager));
  manager->createTask(thread_manager, "manager",
                      configMINIMAL_STACK_SIZE * 10,tskIDLE_PRIORITY + 1UL,
                      static_cast<void*>(qmanager));
  manager->createTask(thread_rotary, "rotary",
                      configMINIMAL_STACK_SIZE * 10,tskIDLE_PRIORITY + 1UL,
                      static_cast<void*>(qmanager));
  manager->createTask(thread_user_interface, "user_interface",
                      configMINIMAL_STACK_SIZE * 10,tskIDLE_PRIORITY + 1UL,
                      static_cast<void*>(qmanager));
  
  LOG_INFO("Start the real time kernel with preemption");
  vTaskStartScheduler ();

  return 1;
}
