/*
 * Clock.h
 *
 *  Created on: 14 May 2023
 */

#ifndef PERIPHERALS_CLOCK_H_
#define PERIPHERALS_CLOCK_H_

#include "chip.h"
#include "Fmutex.h"

struct TimeFromStart
{
  uint16_t days;
  uint8_t hours;
  uint8_t minutes;
  uint8_t seconds;
  uint16_t milliseconds;
};


class Clock {
public:
  Clock();
  virtual ~Clock();
  void updateClock();
  TimeFromStart getTimeFromStart();
  TimeFromStart convertToTimeFromStart(double raw_time);
  double getTimeFromStartRaw();
  double convertToTimeFromStartRaw(TimeFromStart tfs);
private:
  double _raw_time; //ms
  int64_t _overflows;
  uint32_t _last_counter_value;
  Fmutex _guard;
};

#endif /* PERIPHERALS_CLOCK_H_ */
