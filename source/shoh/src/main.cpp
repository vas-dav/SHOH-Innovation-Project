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

  ThreadCommon::ThreadManager *tm = new ThreadCommon::ThreadManager();
  ThreadCommon::QueueManager *qm = new ThreadCommon::QueueManager();
  ThreadCommon::CommonManagers *manager = new ThreadCommon::CommonManagers;
  manager->tm = tm;
  manager->qm = qm;
  manager->tm->createTask(thread_master, "master",
                        configMINIMAL_STACK_SIZE * 10,tskIDLE_PRIORITY + 1UL,
                        static_cast<void*>(manager));
  vTaskStartScheduler ();

  return 1;
}

extern "C"
{
  void
  vConfigureTimerForRunTimeStats (void)
  {
    Chip_SCT_Init (LPC_SCT1);
    LPC_SCT1->CONFIG = SCT_CONFIG_32BIT_COUNTER;
    LPC_SCT1->CTRL_U = SCT_CTRL_PRE_L (255)
                            | SCT_CTRL_CLRCTR_L; // set prescaler to 256 (255 +
                                                 // 1), and start timer
  }
}
