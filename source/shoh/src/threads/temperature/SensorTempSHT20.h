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
  int8_t getTemperature();
  bool is_up();
private:
  uint16_t read();
  bool crc_check(uint8_t * data, uint8_t nbrOfBytes, uint8_t checksum);

  I2C* _pi2c;
  const uint8_t _dev_addr;
  bool _up_flag;
  const uint8_t _com_read_hold;
  const uint8_t _com_read_nohold;
  const uint8_t _com_write_ur; //user register
  const uint8_t _com_read_ur; //user register
  const uint8_t _com_soft_reset;
  const uint16_t _polynominal;
};

#endif /* THREADS_TEMPERATURE_SENSORTEMPSHT20_H_ */
