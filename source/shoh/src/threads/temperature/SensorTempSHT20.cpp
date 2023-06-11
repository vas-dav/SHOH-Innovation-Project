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
: _pi2c(pi2c), _dev_addr(0x40)
{}

SensorTempSHT20::~SensorTempSHT20()
{}
/*
const int16_t POLYNOMIAL = 0x131;

int8_t SHT2x_CheckCrc(int8_t data[], int8_t nbrOfBytes, int8_t checksum)
{
    int8_t crc = 0;
    int8_t bit;
    int8_t byteCtr;
    //calculates 8-Bit checksum with given polynomial
    for (byteCtr = 0; byteCtr < nbrOfBytes; ++byteCtr)
    {
        crc ^= (data[byteCtr]);
        for ( bit = 8; bit > 0; --bit)
        {
            if (crc & 0x80) crc = (crc << 1) ^ POLYNOMIAL;
            else crc = (crc << 1);
        }
    }
    if (crc != checksum) return 1;
    else return 0;
}
*/
