#include "chip.h"
#include "board.h"
#include "FreeRTOS.h"
#include "task.h"
#include <cr_section_macros.h>
#include "ThreadCommon.h"
#include "Master.h"
#include "Rotary.h"
#include "retarget_uart.h"
#include "LiquidCrystal.h"
#include "Manager.h"

void lcd_starting();

int main(void)
{
  SystemCoreClockUpdate();
  Board_Init();

  retarget_init();

  printf("Hello there!\r\n");
  
  lcd_starting();

  ThreadCommon::ThreadManager* manager = new ThreadCommon::ThreadManager;
  ThreadCommon::QueueManager* qmanager = new ThreadCommon::QueueManager;
  //Creating queues
  qmanager->createQueue(100,
                        sizeof(Event),
                        ThreadCommon::QueueManager::master_event_all);
  qmanager->createQueue(20,
                        sizeof(Event),
                        ThreadCommon::QueueManager::manager_event_master);
  
  //Creating tasks
  manager->createTask(master_thread, "master",
                      configMINIMAL_STACK_SIZE * 10,tskIDLE_PRIORITY + 1UL,
                      static_cast<void*>(qmanager));
  manager->createTask(thread_manager, "manager",
                      configMINIMAL_STACK_SIZE * 10,tskIDLE_PRIORITY + 1UL,
                      static_cast<void*>(qmanager));
  manager->createTask(rotary_thread, "rotary",
                      configMINIMAL_STACK_SIZE * 10,tskIDLE_PRIORITY + 1UL,
                      static_cast<void*>(qmanager));
  
  // Start the real time kernel with preemption.
  vTaskStartScheduler ();

  return 1;
}

/**
 * @brief While we don't have UI, this should do for LCD.
 * 
 */
void lcd_starting() {
  // LCD pins init.
  //Well, this lpc is a bit different with those. Table 83 from UM10732-11u68.pdf
  DigitalIoPin *rs = new DigitalIoPin(1, 9, false);
  Chip_IOCON_PinMuxSet (LPC_IOCON, 1, 9, (IOCON_FUNC0 | IOCON_MODE_INACT | IOCON_DIGMODE_EN));

  DigitalIoPin *en = new DigitalIoPin(0, 14, false);
  Chip_IOCON_PinMuxSet (LPC_IOCON, 0, 14, (IOCON_FUNC1 | IOCON_MODE_INACT | IOCON_DIGMODE_EN));

  DigitalIoPin *d4 = new DigitalIoPin(0, 13, false);
  Chip_IOCON_PinMuxSet (LPC_IOCON, 0, 13, (IOCON_FUNC1 | IOCON_MODE_INACT | IOCON_DIGMODE_EN));

  DigitalIoPin *d5 = new DigitalIoPin(0, 12, false);
  Chip_IOCON_PinMuxSet (LPC_IOCON, 0, 12, (IOCON_FUNC1 | IOCON_MODE_INACT | IOCON_DIGMODE_EN));

  DigitalIoPin *d6 = new DigitalIoPin(0, 23, false);
  Chip_IOCON_PinMuxSet (LPC_IOCON, 0, 23, (IOCON_FUNC0 | IOCON_MODE_INACT | IOCON_DIGMODE_EN));

  DigitalIoPin *d7 = new DigitalIoPin(0, 11, false);
  Chip_IOCON_PinMuxSet (LPC_IOCON, 0, 11, (IOCON_FUNC1 | IOCON_MODE_INACT | IOCON_DIGMODE_EN));

  rs->write(false);
  en->write(false);
  d4->write(false);
  d5->write(false);
  d6->write(false);
  d7->write(false);
  // LCD init.
  LiquidCrystal *lcd = new LiquidCrystal(rs, en, d4, d5, d6, d7);
  // LCD configure display geometry.
  lcd->begin(16, 2);
  lcd->setCursor (0, 0);
  lcd->print("Hello there");
  lcd->setCursor (0, 1);
  lcd->print("Starting...");
}
