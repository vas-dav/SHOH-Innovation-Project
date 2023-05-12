/*
 * Relay.cpp
 *
 *  Created on: 12 May 2023
 *      Author: tylen
 */

#include "Relay.h"
#include "Event.h"
#include "Log.h"

RelayDevice::RelayDevice(uint8_t pha_pin, 
						 uint8_t pha_port, 
						 uint8_t relay_device_index,
						 bool heat_output_1
						 ) :
						 heat_output_one(heat_output_1)
{
	pha = new DigitalIoPin(pha_pin, pha_port, false);
	LOG_DEBUG("Creating RelayDevice with output %d", 
			  heat_output_one ? 1 : 2);
	pha->write(heat_output_one);
}

RelayDevice::~RelayDevice() 
{
	LOG_ERROR("Deleting RelayDevice");
}

void inline RelayDevice::RelayOn()
{	
	pha->write(!heat_output_one);	
}

void inline RelayDevice::RelayOff()
{
	pha->write(heat_output_one);
}


Relay::Relay(ThreadCommon::QueueManager* qm): _qm(qm) 
{
	LOG_DEBUG("Creating Relay");
}

Relay::~Relay() 
{
	LOG_ERROR("Deleting Relay");
}

void Relay::setPowerMode(PowerMode pm)
{
	switch (pm)
	{
	case PowerMode::POWER_0:
		relays[INF_RELAY].RelayOff();
		relays[SUP_RELAY].RelayOff();
		LOG_INFO("Heater is turned OFF");
	case PowerMode::POWER_1:
		relays[INF_RELAY].RelayOn();
		relays[SUP_RELAY].RelayOff();
		LOG_INFO("Heater using power mode 1");
		break;
	case PowerMode::POWER_2:
		relays[INF_RELAY].RelayOff();
		relays[SUP_RELAY].RelayOn();
		LOG_INFO("Heater using power mode 2");
		break;
	case PowerMode::POWER_3:
		relays[INF_RELAY].RelayOn();
		relays[SUP_RELAY].RelayOn();
		LOG_INFO("Heater using power mode 3");
		break;
	default:
		break;
	}
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
			EventRawData rd = d->getDataOf(static_cast<Event::EventType>(i));
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
					break;
				default:
					assert(0);
					break;
			}
		
		}
}

void thread_relay(void * pvParams)
{
	ThreadCommon::CommonManagers * manager = static_cast<ThreadCommon::CommonManagers*>(pvParams);
	Relay r(manager->qm);
}
