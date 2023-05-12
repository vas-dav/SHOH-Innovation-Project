/*
 * Relay.cpp
 *
 *  Created on: 12 May 2023
 *      Author: tylen
 */

#include "Relay.h"
#include "Event.h"
#include "Log.h"

// TODO Remove en pins
RelayDevice::RelayDevice(uint8_t en_pin, 
						 uint8_t en_port, 
						 uint8_t pha_pin, 
						 uint8_t pha_port, 
						 uint8_t relay_device_index
						 )
{
	en = new DigitalIoPin(en_pin, en_port, false);
	pha = new DigitalIoPin(pha_pin, pha_port, false);
	LOG_DEBUG("Creating RelayDevice");
}

RelayDevice::~RelayDevice() 
{
	LOG_ERROR("Deleting RelayDevice");
}

Relay::Relay(ThreadCommon::QueueManager* qm): _qm(qm) 
{
	LOG_DEBUG("Creating Relay");
}

Relay::~Relay() 
{
	LOG_ERROR("Deleting Relay");
}

void Relay::taskFunction()
{
	Event data(Event::Null, 0);
	for(;;)
	{
		_qm->receive<Event>(ThreadCommon::QueueManager::relay_event_master, &data, portMAX_DELAY);
		parseEvent(&data);
	}
}

void Relay::parseEvent(Event* d)
{
	for (uint8_t i = Event::ExternalTemp; i <= Event::SetPoint; i++)
		{
			EventRawData rd = data.getDataOf(i);
			if(rd == ERROR_RETURN)
			{
				continue;
			}
			switch(i /* EventType */)
			{
				case Event::ExternalTemp:
					ext_temp = rd;
					break;
				case Event::SetPoint:
					setpoint = rd;
				default:
					assert(0);
			}
		
		}
}

void thread_relay(void * pvParams)
{
	ThreadCommon::CommonManagers * manager = static_cast<ThreadCommon::CommonManagers*>(pvParams);
	Relay r(manager->qm);
}