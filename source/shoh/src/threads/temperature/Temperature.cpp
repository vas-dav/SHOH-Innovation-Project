/*
 * Temperature.cpp
 *
 *  Created on: 16 May 2023
 */

#include "Temperature.h"
#include "SensorTempTC74.h"
#include "Event.h"
#include "Log.h"

Temperature::Temperature(ThreadCommon::QueueManager* qm, I2C* pi2c) : _qm(qm), _pi2c(pi2c) {}

Temperature::~Temperature() {}

void Temperature::taskFunction()
{
	SensorTempTC74 ext_temp_sensor(this->_pi2c, 0x4a);
	Event t (Event::ExternalTemp, -10);
	int8_t temp_value = -10;
	for (;;)
	{
		if (ext_temp_sensor.is_up())
			temp_value = ext_temp_sensor.getTemperature();

		if(temp_value == -10) 
		{
			LOG_ERROR("Failed to get temperature.");
			continue;
		}

		LOG_DEBUG("External temperature is: %d", temp_value);
		t.setDataOf(Event::ExternalTemp, temp_value);
		_qm->send<Event>(ThreadCommon::QueueManager::master_event_all, &t, 0);
		vTaskDelay(5000);
	}
}

void thread_temperature(void* pvParams)
{
	ThreadCommon::CommonManagers * manager = static_cast<ThreadCommon::CommonManagers*>(pvParams);
	I2C_config conf{0x4a, 100000};
	I2C i2c(conf);
	Temperature t(manager->qm, &i2c);
	t.taskFunction();
}
