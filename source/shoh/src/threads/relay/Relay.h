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

class RelayDevice {
	public:
		RelayDevice(uint8_t pha_pin, 
					uint8_t pha_port, 
					uint8_t relay_device_index);
		virtual ~RelayDevice();
	private:
		DigitalIoPin * pha;
};

class Relay {
public:
	Relay(ThreadCommon::QueueManager* qm);
	virtual ~Relay();
	void taskFunction();
private:
	ThreadCommon::QueueManager* _qm;
	RelayDevice relays [2] = {{0, 24, 0},
							  {0, 26, 1}};

	void parseEvent(Event * d);
	int8_t setpoint, ext_temp;
};

void thread_relay(void * pvParams);

#endif /* THREADS_RELAY_RELAY_H_ */
