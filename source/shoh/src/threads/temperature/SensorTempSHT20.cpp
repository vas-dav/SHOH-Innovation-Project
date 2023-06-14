/*
 * SensorTempSHT20.cpp
 *
 *  Created on: 16 May 2023
 */

#include "SensorTempSHT20.h"
#include "FreeRTOS.h"
#include "task.h"
#include "Log.h"

//D6U9H:
//D - Digital (i2c)
//6 - 2016 year of production
//U9H - Sensirion undecodable crap
//Address: 0x40

//Trigger T measurement | hold master    | 1110 0011 - 0xe3 (implemented)
//Trigger T measurement | no hold master | 1111 0011 - 0xf3
//Write user register   |                | 1110 0110 - 0xe6
//Read user register    |                | 1110 0111 - 0xe7
//Soft reset            |                | 1111 1110 - 0xfe


SensorTempSHT20::SensorTempSHT20(I2C* pi2c)
: _pi2c(pi2c), _dev_addr(0x40), _up_flag(false),
_com_read_hold(0xe3), _com_read_nohold(0xf3),
_com_write_ur(0xe6), _com_read_ur(0xe7),
_com_soft_reset(0xfe), _polynominal(0x131)
{
  //Read sensor during the initialisation to get the "UP" state earlier.
  this->read();
}

SensorTempSHT20::~SensorTempSHT20()
{}

/**
 * @brief Gets temperature from SHT20 sensor.
 * 
 * @return int8_t temperature value trimmed from -128 to 127.
 */
int8_t SensorTempSHT20::getTemperature()
{
  uint16_t raw_temp = this->read();
  bool err_bit = raw_temp & 0x1;
  int temp = 0;
  if (err_bit)
    return -128;

  //Formula: (St / 2 ^ 16) * 175.72 - 46.85
  temp = ((double)raw_temp / 65536) * 175.72 - 46.85;

  if(temp > 127)
    temp = 127;
  if (temp < -128)
    temp = -128;

  return temp;
}

/**
 * @brief Makes sure that the sensor is up.
 * 
 * @return true It is.
 * @return false It is not.
 */
bool SensorTempSHT20::is_up()
{
  this->read();
  return this->_up_flag;
}

//Use hold master - read(com = 0xe3)
//Reading is done via 3 bytes.
//WRITE: I2C address + write | read command
//READ: Data (MSB) | Data (LSB) + Stat. | Checksum
/**
 * @brief Gets raw temperature measurement data from sensor
 * using hold master mode.
 * 
 * @return uint16_t Raw temperature measurement data.
 */
uint16_t SensorTempSHT20::read()
{
  uint8_t  tbuf = this->_com_read_hold;
  uint8_t  rbuf[3] = {0x0, 0x2, 0x0};
  uint8_t crc = 0x0;
  uint16_t  raw_temp = 0;
  this->_up_flag = this->_pi2c->transaction(this->_dev_addr, &tbuf, 1, rbuf, 3);
  
  //Sensor changes this bit to 0 on temp measurement.
  if (rbuf[1] & 0x2)
    return 0x1;

  raw_temp |= (rbuf[0] << 8);
  raw_temp |= (rbuf[1] & 0xfc);
  crc = rbuf[2];

  if (this->crc_check(rbuf, 2, crc))
    LOG_WARNING("Temperature sensor reported crc mismatch. Raw data: %04x; CRC: %x", raw_temp, crc);
  
  return raw_temp;
}

/**
 * @brief Checks if checksum is correct.
 * 
 * @param data array of raw data to check.
 * @param nbrOfBytes size of an array.
 * @param checksum received checksum to compare calculated crc to.
 * @return true - checksum is incorrect.
 * @return false - checksum is the same as calculated.
 */
bool SensorTempSHT20::crc_check(uint8_t * data, uint8_t nbrOfBytes, uint8_t checksum)
{
  uint8_t crc = 0;
  uint8_t bit;
  uint8_t byteCtr;
  //Calculates 8-Bit checksum with given _polynomial
  for (byteCtr = 0; byteCtr < nbrOfBytes; ++byteCtr)
  {
    crc ^= (data[byteCtr]);
    for ( bit = 8; bit > 0; --bit)
      crc = (crc & 0x80) ? ((crc << 1) ^ this->_polynominal) : (crc << 1);
  }
  return crc != checksum;
}
