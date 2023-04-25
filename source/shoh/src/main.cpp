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
  manager.createTask(master_thread, "master",
                     configMINIMAL_STACK_SIZE * 10,tskIDLE_PRIORITY + 1UL,
                     nullptr);
  // Start the real time kernel with preemption.
  //FreeRTOS::Kernel::startScheduler();
  vTaskStartScheduler ();

  return 1;
}
