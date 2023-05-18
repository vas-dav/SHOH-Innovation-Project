/*
 * Relay.h
 *
 *  Created on: 12 May 2023
 *      Author: tylen
 */

#ifndef THREADS_RELAY_RELAY_H_
#define THREADS_RELAY_RELAY_H_

#include "ThreadCommon.h"
#include "DigitalIoPin.h"
#include "Counter.h"
#include "Event.h"

class RelayDevice {
	public:
		RelayDevice(uint8_t pha_pin, 
					uint8_t pha_port, 
					uint8_t relay_device_index,
					bool heat_output_1 = true);
		virtual ~RelayDevice();
		void RelayOn();
		void RelayOff();
	private:
		DigitalIoPin * pha;
		bool heat_output_one;
};

class Relay {
	enum PowerMode 
	{
		POWER_0,
		POWER_1,
		POWER_2,
		POWER_3
	};
	enum RelayType 
	{
		INF_RELAY,
		SUP_RELAY
	};
public:
	Relay(ThreadCommon::QueueManager* qm);
	virtual ~Relay();
	void taskFunction();
	void setPowerMode(PowerMode pm);
	void utilizeEventData();
private:
	ThreadCommon::QueueManager* _qm;
	RelayDevice relays [2] = {{0, 24, 0},
							  {0, 26, 1}};

	void parseEvent(Event * e);
	int8_t setpoint, ext_temp;
};

void thread_relay(void * pvParams);

#endif /* THREADS_RELAY_RELAY_H_ */
