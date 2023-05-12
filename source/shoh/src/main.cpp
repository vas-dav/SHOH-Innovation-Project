#include "chip.h"
#include "board.h"
#include "FreeRTOS.h"
#include "task.h"
#include <cr_section_macros.h>
#include "retarget_uart.h"

#include "ThreadCommon.h"
#include "Master.h"



int main(void)
{
  SystemCoreClockUpdate();
  Board_Init();

  retarget_init();

  ThreadCommon::CommonManagers manager = {new ThreadCommon::ThreadManager,
                                          new ThreadCommon::QueueManager};
  manager.tm->createTask(thread_master, "master",
                        configMINIMAL_STACK_SIZE * 10,tskIDLE_PRIORITY + 1UL,
                        static_cast<void*>(&manager));
  vTaskStartScheduler ();

  return 1;
}
