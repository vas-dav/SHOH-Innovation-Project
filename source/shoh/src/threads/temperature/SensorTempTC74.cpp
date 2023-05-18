/*
 * SensorTempTC74.cpp
 *
 *  Created on: 16 May 2023
 */

#include "SensorTempTC74.h"
#include "FreeRTOS.h"
#include "task.h"
#include "Log.h"

SensorTempTC74::SensorTempTC74(I2C* pi2c, const uint8_t dev_addr)
: _pi2c(pi2c), _dev_addr(dev_addr), _temp_reg(0x00), _ctrl_reg(0x01),
  _ready_bit(0x40), _standby_bit(0x80), _up_flag(false)
{}

SensorTempTC74::~SensorTempTC74()
{}

int8_t SensorTempTC74::getTemperature()
{
  return static_cast<int8_t>(this->read()) - 6;
}

bool SensorTempTC74::is_up()
{
  this->on_standby();
  if (!this->_up_flag)
    LOG_WARNING("Unable to reach temperature sensor [%d]", this->_dev_addr);
  return this->_up_flag;
}

bool SensorTempTC74::on_standby()
{
  uint8_t data = 0x00;
  this->_up_flag = this->read_reg(this->_ctrl_reg, &data, 1);
  return data & this->_standby_bit;
}

bool SensorTempTC74::is_ready()
{
  uint8_t data = 0x00;
  this->_up_flag = this->read_reg(this->_ctrl_reg, &data, 1);
  return data & this->_ready_bit;
}

uint8_t SensorTempTC74::read()
{
  uint8_t data = 0x80; //  b(1000 0000)
  //if on standby - remove standby and wait for 1 ms.
  if (this->on_standby())
  {
    this->remove_standby();
    vTaskDelay(3000);
  }
  
  //if ready and up - read
  if (this->is_ready() && this->_up_flag)
  {
    this->_up_flag = this->read_reg(this->_temp_reg, &data, 1);
    if(!this->_up_flag)
      LOG_WARNING("I2C transaction for getting the temperature failed.");
    LOG_DEBUG("Temperature sensor returned: %x", data);
  }
  else
    LOG_WARNING("Unable to read temperature sensor [%d] value.", this->_dev_addr);
  
  //set standy.
  //this->set_standby();

  return data;
}

void SensorTempTC74::remove_standby()
{
  uint8_t data = 0x00;
  this->_up_flag = this->write_reg(this->_ctrl_reg, &data, 1);
  if(!this->_up_flag)
    LOG_WARNING("Unable to remove standby for temperature sensor [%d].", this->_dev_addr);
}

void SensorTempTC74::set_standby()
{
  uint8_t data = this->_standby_bit;
  this->_up_flag = this->write_reg(this->_ctrl_reg, &data, 1);
  if(!this->_up_flag)
    LOG_WARNING("Unable to set standby for temperature sensor [%d].", this->_dev_addr);
}

bool SensorTempTC74::write_reg(uint8_t com, uint8_t *trdata, const uint16_t size)
{
  vTaskDelay(1); //Not sure if it is needed.
  uint8_t arr[size + 1];
	arr[0] = com;
	for (unsigned int i = 1; i < (unsigned int)size + 1; i++) {
		arr[i] = trdata[i - 1];
	}
	return this->_pi2c->write(this->_dev_addr, arr, size + 1);
}

bool SensorTempTC74::read_reg(uint8_t com, uint8_t *rdata, uint16_t size)
{
  vTaskDelay(1); //Not sure if it is needed.
  return this->_pi2c->transaction(this->_dev_addr, &com, 1, rdata, size);
}
