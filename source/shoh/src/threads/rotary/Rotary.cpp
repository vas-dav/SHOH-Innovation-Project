/*
 * Rotary.cpp
 *
 *  Created on: 26 Apr 2023
 *      Author: tylen
 */

#include "Rotary.h"
#include "board.h"
#include "queue.h"
#include "Log.h"

static QueueHandle_t * p_rotary_isr_q;

static DigitalIoPin * p_sigB;

extern "C" 
{
  void
  PIN_INT0_IRQHandler (void)
  {
	Chip_PININT_ClearIntStatus (LPC_PININT, PININTCH (PIN_INT0_IRQn));
	portBASE_TYPE xHigherPriorityWoken = pdFALSE;
	uint8_t data;

	if (p_sigB->read())
		data = ThreadCommon::RotaryAction::Left;
	else
		data = ThreadCommon::RotaryAction::Right;

	xQueueSendFromISR (*p_rotary_isr_q, &data,  &xHigherPriorityWoken);
	portEND_SWITCHING_ISR(xHigherPriorityWoken);
  }
/*
  void
  PIN_INT1_IRQHandler (void)
  {
    Chip_PININT_ClearIntStatus (LPC_PININT, PININTCH (PIN_INT1_IRQn));
	portBASE_TYPE xHigherPriorityWoken = pdFALSE;
	uint8_t data = ThreadCommon::RotaryAction::Left;
	xQueueSendFromISR (*p_rotary_isr_q, &data,  &xHigherPriorityWoken);
	portEND_SWITCHING_ISR(xHigherPriorityWoken);
  }
*/
  void
  PIN_INT1_IRQHandler (void)
  {
    Chip_PININT_ClearIntStatus (LPC_PININT, PININTCH (PIN_INT1_IRQn));
	portBASE_TYPE xHigherPriorityWoken = pdFALSE;
	uint8_t data = ThreadCommon::RotaryAction::Press;
	xQueueSendFromISR (*p_rotary_isr_q, &data,  &xHigherPriorityWoken);
	portEND_SWITCHING_ISR(xHigherPriorityWoken);
  }
}

Rotary::Rotary(ThreadCommon::QueueManager* qm) : _qm(qm) 
{
	LOG_DEBUG("Creating Rotary");
	p_sigB = &(this->signal[1]);
}

Rotary::~Rotary() 
{
	LOG_ERROR("Deleting Rotary");
}

void Rotary::taskFunction()
{
	auto action_from_rotary_isr = ThreadCommon::RotaryAction::Idle;
	Event data(Event::EventType::Rotary, action_from_rotary_isr);
	
	for (;;)
	{
		xQueueReceive(*p_rotary_isr_q, &action_from_rotary_isr, portMAX_DELAY);
		data.setEvent(Event::EventType::Rotary, action_from_rotary_isr);
		_qm->send<Event>(ThreadCommon::QueueManager::master_event_all, &data, 10);
	}
}

void thread_rotary(void* pvParams)
{
	// Special case for ISR
	QueueHandle_t rotary_isr_q = xQueueCreate(15, sizeof(char));
	p_rotary_isr_q = &rotary_isr_q;

	ThreadCommon::CommonManagers * manager = static_cast<ThreadCommon::CommonManagers*>(pvParams);
	Rotary r(manager->qm);
	r.taskFunction();
}
