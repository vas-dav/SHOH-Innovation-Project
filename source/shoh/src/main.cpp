#include "chip.h"
#include "board.h"
#include "FreeRTOS.h"
#include "task.h"
#include <cr_section_macros.h>
#include "common/ThreadCommon.h"
#include "Master.h"


int main(void)
{
  SystemCoreClockUpdate();
  Board_Init();
  ThreadCommon::ThreadManager manager;
  ThreadCommon::QueueManager* qmanager = new ThreadCommon::QueueManager;
  //Creating queues
  qmanager->createQueue(100,
                        sizeof(ThreadCommon::Event),
                        ThreadCommon::QueueManager::master_event_all);

  //Creating tasks
  manager.createTask(master_thread, "master",
                     configMINIMAL_STACK_SIZE * 10,tskIDLE_PRIORITY + 1UL,
                     static_cast<void*>(qmanager));
  
  //<Queue_test>
  QueueHandle_t master_event_all_q = qmanager->getQueue(ThreadCommon::QueueManager::master_event_all);
  ThreadCommon::Event* e = new ThreadCommon::Event(ThreadCommon::Null, 0);

  xQueueSend(master_event_all_q, static_cast<void *>(e), 0);
  //</Queue_test>

  // Start the real time kernel with preemption.
  //FreeRTOS::Kernel::startScheduler();
  vTaskStartScheduler ();

  return 1;
}
