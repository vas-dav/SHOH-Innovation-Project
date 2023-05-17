/*
 * Temperature.cpp
 *
 *  Created on: 16 May 2023
 */

#include "Temperature.h"
#include "SensorTempTC74.h"
#include "Log.h"

Temperature::Temperature(ThreadCommon::QueueManager* qm, I2C* pi2c) : _qm(qm), _pi2c(pi2c) {}

Temperature::~Temperature() {}

void Temperature::taskFunction()
{
	SensorTempTC74 ext_temp_sensor(this->_pi2c, 0x4a);
	int8_t temp_value = 0;
	for (;;)
	{
		if (ext_temp_sensor.is_up())
			temp_value = ext_temp_sensor.getTemperature();
		
		LOG_DEBUG("External temperature is: %d", temp_value);
		
		//Send temperature on queue. As event. :(
		
		
		vTaskDelay(5000);
	}
}

void thread_temperature(void* pvParams)
{
	ThreadCommon::CommonManagers * manager = static_cast<ThreadCommon::CommonManagers*>(pvParams);
	I2C_config conf{0x4a, 55000};
	I2C i2c(conf);
	Temperature t(manager->qm, &i2c);
	t.taskFunction();
}
