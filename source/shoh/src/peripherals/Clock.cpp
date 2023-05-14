/*
 * Clock.cpp
 *
 *  Created on: 14 May 2023
 */

#include "Clock.h"
#include "FreeRTOS.h"
#include "task.h"

static uint64_t counter_overflows = 0;

extern "C" {
  /**
   * @brief Interrupt is used by the Clock class to monitor
   * the time elapsed since the start.
   * It triggers on counter overflow.
   */
	void SCT0_1_IRQHandler (void)
	{
		counter_overflows++;
		portBASE_TYPE xHigherPriorityWoken = pdFALSE;
		Chip_SCT_ClearEventFlag(LPC_SCT1, SCT_EVT_0);
		portEND_SWITCHING_ISR(xHigherPriorityWoken);
	}
}

/**
 * @brief Construct a new Clock:: Clock object
 * @attention Should be initialised by the first task as early as possible.
 * _overflows(1) is important, since there is a starting overflow.
 */
Clock::Clock() : _raw_time(0), _overflows(1) {}

Clock::~Clock() {}

/**
 * @brief Handles counter overflows and updates clock value.
 * Time difference between updateClock() calls should not be overly
 * large, if it will be enough to get overflow amount of overflows
 * 2 times between the function calls the clock will desync.
 * *Barely possible with counter overflows every 89 sec, but still.
 */
void Clock::updateClock()
{
  int64_t diff = 0;
  //Remember old number of overflows.
  int64_t old_overflows = this->_overflows;
  //Capture number of counter overflows.
  this->_overflows = counter_overflows;
  
  //Handle overflows.
  diff = (old_overflows <= this->_overflows)
       ? (this->_overflows - old_overflows)
       : (0xffffffffffffffff - old_overflows + this->_overflows + 1);
  _raw_time += (double)((double)diff * (uint32_t)0xffffffff + LPC_SCT1->COUNT_U) * 1000
                        / (double)(Chip_Clock_GetMainClockRate());
}

TimeFromStart Clock::getTimeFromStart()
{
  this->updateClock();
  return this->convertToTimeFromStart(this->_raw_time);
}

TimeFromStart Clock::convertToTimeFromStart(double raw_time)
{
  //The convertion is slow, but reliable.
  TimeFromStart tfs;
  tfs.days = raw_time / 86400000;
  tfs.hours = raw_time / 3600000 - tfs.days * 24; //Can be ((long long int)raw_time % 86400000)
  tfs.minutes = raw_time / 60000 - tfs.days * 1440
                                 - tfs.hours * 60; //Can be ((long long int)raw_time % 3600000)
  tfs.seconds = raw_time / 1000 - tfs.days * 86400
                                - tfs.hours * 3600
                                - tfs.minutes * 60; //Can be ((long long int)raw_time % 60000)
  tfs.milliseconds = raw_time - tfs.days * 86400000
                              - tfs.hours * 3600000
                              - tfs.minutes * 60000
                              - tfs.seconds * 1000; //Can be ((long long int)raw_time % 1000)
  return tfs;
}

double Clock::getTimeFromStartRaw()
{
  this->updateClock();
  return this->_raw_time;
}

double Clock::convertToTimeFromStartRaw(TimeFromStart tfs)
{
  return (double)(tfs.days * 86400000 + tfs.hours * 3600000
                                      + tfs.minutes * 60000
                                      + tfs.seconds * 1000
                                      + tfs.milliseconds);
}

extern "C"
{
  /** 
   * @brief This function is used by FreeRTOS to configure the collection of
   * time used by tasks.
   * .
   */
  void
  vConfigureTimerForRunTimeStats (void)
  {
    Chip_SCT_Init (LPC_SCT1);
    /* Sets match to trigger when all bits of counter are 1 */
    Chip_SCT_SetMatchCount(LPC_SCT1, SCT_MATCH_0, 0xffffffff);
    /* Set corresponding match register to event */
    LPC_SCT1->EVENT[0].CTRL |= 0x1 | (0x1 << 12);
    /* Enable event */
    LPC_SCT1->EVENT[0].STATE |= 0x1;

    /* Set the interrupt */
    Chip_SCT_EnableEventInt(LPC_SCT1, SCT_EVT_0);

    /* Clear timer of any pending interrupts */
    NVIC_ClearPendingIRQ(SCT0_1_IRQn);

    /* Enable timer interrupt */
    NVIC_EnableIRQ(SCT0_1_IRQn);

    /* Start timer */
    LPC_SCT1->CONFIG = SCT_CONFIG_32BIT_COUNTER;
    LPC_SCT1->CTRL_U = SCT_CTRL_CLRCTR_L;
  }
}
