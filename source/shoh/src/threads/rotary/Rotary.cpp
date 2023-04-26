/*
 * Rotary.cpp
 *
 *  Created on: 26 Apr 2023
 *      Author: tylen
 */

#include "Rotary.h"
#include "board.h"

extern "C" 
{
  void
  PIN_INT0_IRQHandler (void)
  {
	Chip_PININT_ClearIntStatus (LPC_PININT, PININTCH (PIN_INT0_IRQn));
  }

  void
  PIN_INT1_IRQHandler (void)
  {
    Chip_PININT_ClearIntStatus (LPC_PININT, PININTCH (PIN_INT1_IRQn));
  }

  void
  PIN_INT2_IRQHandler (void)
  {
    Chip_PININT_ClearIntStatus (LPC_PININT, PININTCH (PIN_INT2_IRQn));
  }
}

Rotary::Rotary(ThreadCommon::QueueManager* qm) : _qm(qm) 
{

}

Rotary::~Rotary() {}

void Rotary::taskFunction()
{
Event data(Event::Null, 0);
	Event* e = new Event(Event::Rotary, ThreadCommon::RotaryAction::Idle);
	_qm->send<Event>(ThreadCommon::QueueManager::master_event_all, e, 10);
	for (;;)
	{
		vTaskDelay(500);
	}
}

void rotary_thread(void* pvParams)
{
	Rotary r(static_cast<ThreadCommon::QueueManager*>(pvParams));
	r.taskFunction();
}
