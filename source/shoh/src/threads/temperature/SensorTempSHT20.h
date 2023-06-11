/*
 * SensorTempSHT20.h
 *
 *  Created on: 16 May 2023
 */

#ifndef THREADS_TEMPERATURE_SENSORTEMPTC74_H_
#define THREADS_TEMPERATURE_SENSORTEMPTC74_H_

#include "I2C.h"
#include "chip.h"

class SensorTempSHT20 {
public:
	SensorTempSHT20(I2C* pi2c);
	virtual ~SensorTempSHT20();
private:
	uint16_t read();

	bool write_com(uint8_t com, uint8_t *trdata, const uint16_t size);
	bool read_com(uint8_t com, uint8_t *rdata, uint16_t size);

	I2C* _pi2c;
	const uint8_t _dev_addr;
	bool _up_flag;
};

#endif /* THREADS_TEMPERATURE_SENSORTEMPSHT20_H_ */
