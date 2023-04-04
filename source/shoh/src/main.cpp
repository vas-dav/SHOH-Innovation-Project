#include "chip.h"
#include "board.h"
#include "FreeRTOS.h"
#include "task.h"
#include <cr_section_macros.h>
#include "FreeRTOSCPP/Kernel.hpp"
#include "threads/master/Master.h"

int main(void)
{
  SystemCoreClockUpdate();
  Board_Init();
  // Create a task before starting the kernel.
  Master master;

  // Start the real time kernel with preemption.
  FreeRTOS::Kernel::startScheduler();

  return 1;
}
