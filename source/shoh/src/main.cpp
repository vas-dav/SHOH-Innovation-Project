#include "chip.h"
#include "board.h"
#include "FreeRTOS.h"
#include "task.h"
#include <cr_section_macros.h>
#include "Master.h"

int main(void)
{
  SystemCoreClockUpdate();
  Board_Init();
  // Create a task before starting the scheduler.
  Master master("Master", configMINIMAL_STACK_SIZE * 10, nullptr, tskIDLE_PRIORITY + 1UL);

  // Start the real time kernel with preemption.
  //FreeRTOS::Kernel::startScheduler();
  vTaskStartScheduler ();

  return 1;
}
