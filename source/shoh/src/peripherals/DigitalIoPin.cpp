/*
 * DigitalIoPin.cpp
 *
 *  Created on: Aug 29, 2022
 *      Author: Vasily Davydov
 */

#include "DigitalIoPin.h"

DigitalIoPin::DigitalIoPin (int port, int pin, bool input, bool pullup,
                            bool invert, bool isr, IRQn_Type isr_index)
{
  assert ((port <= UINT8_MAX_VALUE) && (pin <= UINT8_MAX_VALUE));
  _io._port = (uint8_t)port;
  _io._pin = (uint8_t)pin;
  _io._input = input;
  _io._pullup = pullup;
  _io._invert = invert;
  _io.IOCON_mode = IOCON_MODE_INACT;
  _io.IOCON_inv = IOCON_FUNC0;
  if (isr){
    _io.isr_i = isr_index;
    setIsr();
  } else {
    setIoPin ();
  }
}

DigitalIoPin::~DigitalIoPin ()
{
}

void
DigitalIoPin::setIoPin ()
{
  bool direction = true;
  if (_io._input)
    {
      direction = false;
      _io.IOCON_mode = IOCON_MODE_PULLUP;
      if (!_io._pullup)
        {
          _io.IOCON_mode = IOCON_MODE_PULLDOWN;
        }
      if (_io._invert)
        {
          _io.IOCON_inv = IOCON_INV_EN;
        }
    }
  Chip_IOCON_PinMuxSet (LPC_IOCON, _io._port, _io._pin,
                        (_io.IOCON_mode | _io.DigitalEn | _io.IOCON_inv));
  /**	False direction equals input */
  Chip_GPIO_SetPinDIR (LPC_GPIO, _io._port, _io._pin, direction);
}

void
DigitalIoPin::setIsr ()
{
  bool direction = true;
  if (_io._input)
    {
      direction = false;
      _io.IOCON_mode = IOCON_MODE_PULLUP;
      if (!_io._pullup)
        {
          _io.IOCON_mode = IOCON_MODE_PULLDOWN;
        }
      if (_io._invert)
        {
          _io.IOCON_inv = IOCON_INV_EN;
        }
    }
	/* We'll use an optional IOCON filter (0) with a divider of 64 for the
	   input pin to be used for PININT */
	Chip_Clock_SetIOCONFiltClockDiv(0, 64);

  Chip_IOCON_PinMuxSet (LPC_IOCON, _io._port, _io._pin,
						(_io.IOCON_mode | _io.DigitalEn
						| _io.IOCON_inv | IOCON_CLKDIV(0)
						| IOCON_S_MODE(3)));
  /**	False direction equals input */
  Chip_GPIO_SetPinDIR (LPC_GPIO, _io._port, _io._pin, direction);

  /* Enable PININT clock if it was not enabled before */
  if ((LPC_SYSCTL->SYSAHBCLKCTRL & (1 << SYSCTL_CLOCK_PINT)) == 0)
  {
	Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_PINT);
  }
  /* Configure interrupt channel for the GPIO pin in SysCon block */
	Chip_SYSCTL_SetPinInterrupt(_io.isr_i, _io._port, _io._pin);

	/* Configure channel interrupt as edge sensitive and falling edge interrupt */
	Chip_PININT_ClearIntStatus(LPC_PININT, PININTCH(_io.isr_i));
	Chip_PININT_SetPinModeEdge(LPC_PININT, PININTCH(_io.isr_i));
	Chip_PININT_EnableIntLow(LPC_PININT, PININTCH(_io.isr_i));

	/* Enable interrupt in the NVIC */
	NVIC_ClearPendingIRQ(_io.isr_i);
	NVIC_EnableIRQ(_io.isr_i);
}

bool
DigitalIoPin::read ()
{
	bool state = (Chip_GPIO_GetPinState (LPC_GPIO, _io._port, _io._pin));
	return (_io._invert && !_io._input) ? !state : state;
}

void
DigitalIoPin::write (bool value)
{
  assert (!(_io._input));
  Chip_GPIO_SetPinState (LPC_GPIO, _io._port, _io._pin, ((_io._invert) ? !value : value));
}
