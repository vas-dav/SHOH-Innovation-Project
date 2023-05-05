#include "chip.h"
#include "board.h"
#include "FreeRTOS.h"
#include "task.h"
#include <cr_section_macros.h>
#include "ThreadCommon.h"
#include "Master.h"
#include "Rotary.h"
#include "retarget_uart.h"

int main(void)
{
  SystemCoreClockUpdate();
  Board_Init();

  retarget_init();

  printf("Hello there!\r\n");

  ThreadCommon::ThreadManager* manager = new ThreadCommon::ThreadManager;
  ThreadCommon::QueueManager* qmanager = new ThreadCommon::QueueManager;
  //Creating queues
  qmanager->createQueue(100,
                        sizeof(Event),
                        ThreadCommon::QueueManager::master_event_all);
  //Creating tasks
  manager->createTask(master_thread, "master",
                      configMINIMAL_STACK_SIZE * 10,tskIDLE_PRIORITY + 1UL,
                      static_cast<void*>(qmanager));
  manager->createTask(rotary_thread, "rotary",
                      configMINIMAL_STACK_SIZE * 10,tskIDLE_PRIORITY + 1UL,
                      static_cast<void*>(qmanager));
  
  // Start the real time kernel with preemption.
  vTaskStartScheduler ();

  return 1;
}
