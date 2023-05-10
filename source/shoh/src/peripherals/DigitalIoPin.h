/*
 * DigitalIoPin.h
 *
 *  Created on: Aug 29, 2022
 *      Author: Vasily Davydov
 */

#ifndef DIGITALIOPIN_H_
#define DIGITALIOPIN_H_

#define UINT8_MAX_VALUE 255

#include <assert.h>
#include <chip.h>

typedef struct DigitalIOConfigStruct
{
  uint8_t _port;
  uint8_t _pin;
  bool _input;
  bool _pullup;
  bool _invert;
  uint32_t IOCON_mode;
  uint32_t IOCON_inv;
  uint32_t DigitalEn;
  uint32_t IOFunction;
  IRQn_Type isr_i;
} DigitalIOConfigStruct;

class DigitalIoPin
{
public:
  DigitalIoPin (int port, int pin, bool input = true, bool pullup = true,
                bool invert = false, bool isr = false, IRQn_Type isr_index = PIN_INT0_IRQn);
  DigitalIoPin (const DigitalIoPin &) = delete;
  virtual ~DigitalIoPin ();
  bool read ();
  void write (bool value);

private:
  DigitalIOConfigStruct _io = { 0, 0, false, false, false, 0, 0, IOCON_DIGMODE_EN};
  void setIoPin ();
  void setIsr();
};

#endif /* DIGITALIOPIN_H_ */
