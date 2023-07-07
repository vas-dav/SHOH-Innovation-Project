/*
 * pinportmap.h
 *
 *  Created on: 17 Jun 2023
 *      Author: dave
 */

#ifndef THREADS_COMMON_PINPORTMAP_H_
#define THREADS_COMMON_PINPORTMAP_H_

namespace ThreadCommon
{

	enum PINMAP
	{
		PIN_LCD_RS = 24,
		PIN_LCD_EN = 26,
		PIN_LCD_D4 = 27,
		PIN_LCD_D5 = 25,
		PIN_LCD_D6 = 28,
		PIN_LCD_D7 = 3,
		PIN_I2C_SCL = 4,
		PIN_I2C_SDA = 5,
		PIN_ROTARY_SIG_A = 18,
		PIN_ROTARY_SIG_B = 12,
		PIN_ROTARY_PRESS = 2,
		PIN_BOARD_SW1 = 1
	};

	enum PORTMAP
	{
		PORT_LCD_RS = 1,
		PORT_LCD_EN = 1,
		PORT_LCD_D4 = 1,
		PORT_LCD_D5 = 1,
		PORT_LCD_D6 = 1,
		PORT_LCD_D7 = 2,
		PORT_I2C_SCL = 0,
		PORT_I2C_SDA = 0,
		PORT_ROTARY_SIG_A = 1,
		PORT_ROTARY_SIG_B = 2,
		PORT_ROTARY_PRESS = 0,
		PORT_BOARD_SW1 = 0
	};
}



#endif /* THREADS_COMMON_PINPORTMAP_H_ */
