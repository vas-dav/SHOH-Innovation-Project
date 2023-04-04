#include "chip.h"
#include "board.h"
#include "FreeRTOS.h"
#include "task.h"
#include <cr_section_macros.h>
#include "FreeRTOSCPP/Kernel.hpp"
#include "FreeRTOSCPP/Task.hpp"

class LedTask : public FreeRTOS::Task {
 public:
  LedTask(const UBaseType_t priority, const char* name)
      : FreeRTOS::Task(priority, configMINIMAL_STACK_SIZE, name) {}
  void taskFunction() final;
 private:
  int led;
};

// Task to be created.
void LedTask::taskFunction() {
  bool LedState = false;
  for (;;) {
		Board_LED_Set(led, LedState);
		LedState = (bool) !LedState;
		led++;
		if(led > 2){
			led = 0;
		}
		vTaskDelay(configTICK_RATE_HZ / 2);
  }
}


int main(void)
{
  SystemCoreClockUpdate();
  Board_Init();
  // Create a task before starting the kernel.
  LedTask task1((tskIDLE_PRIORITY + 1UL), "vTaskLed1");

  // Start the real time kernel with preemption.
  FreeRTOS::Kernel::startScheduler();

  return 1;
}
