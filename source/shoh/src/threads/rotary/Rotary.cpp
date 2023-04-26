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
    //portEND_SWITCHING_ISR ();
  }

  void
  PIN_INT1_IRQHandler (void)
  {
    //portEND_SWITCHING_ISR ();
  }

  void
  PIN_INT2_IRQHandler (void)
  {
    //portEND_SWITCHING_ISR ();
  }
}
// For example
#define GPIO_PININT_PIN     1	/* GPIO pin number mapped to PININT */
#define GPIO_PININT_PORT    0 /* GPIO port number mapped to PININT */

Rotary::Rotary(ThreadCommon::QueueManager* qm) : _qm(qm) 
{

}

Rotary::~Rotary() {}

void Rotary::taskFunction()
{
Event data(Event::Null, 0);
	Event* e = new Event(Event::Rotary, ThreadCommon::RotaryAction::Idle);
	_qm->send<Event>(ThreadCommon::QueueManager::master_event_all, e, 10);
	for (;;) {}
}

void rotary_thread(void* pvParams)
{
	Rotary r(static_cast<ThreadCommon::QueueManager*>(pvParams));
	r.taskFunction();
}
