/*
 * SensorTempTC74.cpp
 *
 *  Created on: 16 May 2023
 */

#include "SensorTempTC74.h"

SensorTempTC74::SensorTempTC74(I2C* pi2c) : _pi2c(pi2c) {}

SensorTempTC74::~SensorTempTC74() {}
