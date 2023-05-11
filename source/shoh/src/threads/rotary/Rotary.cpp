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

extern "C" 
{
  void
  PIN_INT0_IRQHandler (void)
  {
	Chip_PININT_ClearIntStatus (LPC_PININT, PININTCH (PIN_INT0_IRQn));
	portBASE_TYPE xHigherPriorityWoken = pdFALSE;
	uint8_t data = ThreadCommon::RotaryAction::Right;
	xQueueSendFromISR (*p_rotary_isr_q, &data,  &xHigherPriorityWoken);
	if(!xHigherPriorityWoken)
	{
		LOG_WARNING("[PIN_INT0_IRQn] portEND_SWITCHING_ISR called with False value");
	}
	portEND_SWITCHING_ISR(xHigherPriorityWoken);
  }

  void
  PIN_INT1_IRQHandler (void)
  {
    Chip_PININT_ClearIntStatus (LPC_PININT, PININTCH (PIN_INT1_IRQn));
	portBASE_TYPE xHigherPriorityWoken = pdFALSE;
	uint8_t data = ThreadCommon::RotaryAction::Left;
	xQueueSendFromISR (*p_rotary_isr_q, &data,  &xHigherPriorityWoken);
	if(!xHigherPriorityWoken)
	{
		LOG_WARNING("[PIN_INT1_IRQn] portEND_SWITCHING_ISR called with False value");
	}
	portEND_SWITCHING_ISR(xHigherPriorityWoken);
  }

  void
  PIN_INT2_IRQHandler (void)
  {
    Chip_PININT_ClearIntStatus (LPC_PININT, PININTCH (PIN_INT2_IRQn));
	portBASE_TYPE xHigherPriorityWoken = pdFALSE;
	uint8_t data = ThreadCommon::RotaryAction::Press;
	xQueueSendFromISR (*p_rotary_isr_q, &data,  &xHigherPriorityWoken);
	if(!xHigherPriorityWoken)
	{
		LOG_WARNING("[PIN_INT2_IRQn] portEND_SWITCHING_ISR called with False value");
	}
	portEND_SWITCHING_ISR(xHigherPriorityWoken);
  }
}

Rotary::Rotary(ThreadCommon::QueueManager* qm) : _qm(qm) 
{
	LOG_DEBUG("Creating Rotary");
}

Rotary::~Rotary() 
{
	LOG_ERROR("Deleting Rotary");
}

void Rotary::taskFunction()
{
	auto action_from_rotary_isr = ThreadCommon::RotaryAction::Idle;
	Event * p_e= new Event(Event::EventType::Rotary, action_from_rotary_isr);
	
	for (;;)
	{
		xQueueReceive(*p_rotary_isr_q, &action_from_rotary_isr, portMAX_DELAY);
		p_e->setDataOf(Event::EventType::Rotary, action_from_rotary_isr);
		_qm->send<Event>(ThreadCommon::QueueManager::master_event_all, p_e, 10);
	}
}

void thread_rotary(void* pvParams)
{
	// Special case for ISR
	QueueHandle_t rotary_isr_q = xQueueCreate(15, sizeof(char));
	p_rotary_isr_q = &rotary_isr_q;

	Rotary r(static_cast<ThreadCommon::QueueManager*>(pvParams));
	r.taskFunction();
}
