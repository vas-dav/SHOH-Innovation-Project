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

//Trigger T measurement | hold master    | 1110 0011 - 0xe3
//Trigger T measurement | no hold master | 1111 0011 - 0xf3
//Write user register   |                | 1110 0110 - 0xe6
//Read user register    |                | 1110 0111 - 0xe7
//Soft reset            |                | 1111 1110 - 0xfe

//Use hold master - read(com = 0xe3)
//Reading is done via 3 bytes.
//WRITE: I2C address + write | read command
//READ: Data (MSB) | Data (LSB) + Stat. | Checksum


SensorTempSHT20::SensorTempSHT20(I2C* pi2c)
: _pi2c(pi2c), _dev_addr(0x40), _up_flag(false),
_com_read_hold(0xe3), _com_read_nohold(0xf3),
_com_write_ur(0xe6), _com_read_ur(0xe7),
_com_soft_reset(0xfe), _polynominal(0x131)
{
  this->read();
}

SensorTempSHT20::~SensorTempSHT20()
{}

int8_t SensorTempSHT20::getTemperature()
{
  uint16_t raw_temp = this->read();
  bool err_bit = raw_temp & 0x1;
  if (err_bit)
    return -128;
  
  //TODO: Temperature parsing has to be done here.
  return raw_temp >> 9;
}

bool SensorTempSHT20::is_up()
{
  this->read();
  return this->_up_flag;
}

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
    LOG_WARNING("Temperature sensor reported crc mismatch.\nRaw data: %04x; CRC: %x", raw_temp, crc);
  
  return raw_temp;
}

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
