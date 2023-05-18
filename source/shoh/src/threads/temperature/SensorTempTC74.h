/*
 * SensorTempTC74.h
 *
 *  Created on: 16 May 2023
 */

#ifndef THREADS_TEMPERATURE_SENSORTEMPTC74_H_
#define THREADS_TEMPERATURE_SENSORTEMPTC74_H_

#include "I2C.h"
#include "chip.h"

class SensorTempTC74 {
public:
	SensorTempTC74(I2C* pi2c, const uint8_t dev_addr);
	virtual ~SensorTempTC74();
	int8_t getTemperature();
	bool is_up();
private:
	uint8_t read();
	bool on_standby();
	bool is_ready();

	void remove_standby();
	void set_standby();

	bool write_reg(uint8_t com, uint8_t *trdata, const uint16_t size);
	bool read_reg(uint8_t com, uint8_t *rdata, uint16_t size);

	I2C* _pi2c;
	const uint8_t _dev_addr;
	const uint8_t _temp_reg;
	const uint8_t _ctrl_reg;
	const uint8_t _ready_bit;
	uint8_t _standby_bit;
	bool _up_flag;
};

#endif /* THREADS_TEMPERATURE_SENSORTEMPTC74_H_ */
