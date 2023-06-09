/*
 * I2C.cpp
 *
 *  Created on: 21.2.2016
 *      Author: krl
 *      Based on example provided by NXP Semiconductors. See copyright notice
 * below.
 */

/*
 * @brief I2CM bus master example using polling mode
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2014
 * All rights reserved.
 *
 * @par
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * LPC products.  This software is supplied "AS IS" without any warranties of
 * any kind, and NXP Semiconductors and its licensor disclaim any and
 * all warranties, express or implied, including all implied warranties of
 * merchantability, fitness for a particular purpose and non-infringement of
 * intellectual property rights.  NXP Semiconductors assumes no responsibility
 * or liability for the use of the software, conveys no license or rights under
 * any patent, copyright, mask work right, or any other intellectual property
 * rights in or to any products. NXP Semiconductors reserves the right to make
 * changes in the software without notification. NXP Semiconductors also makes
 * no representation or warranty that such application will be suitable for the
 * specified use without further testing or modification.
 *
 * @par
 * Permission to use, copy, modify, and distribute this software and its
 * documentation is hereby granted, under NXP Semiconductors' and its
 * licensor's relevant copyrights in the software, without fee, provided that
 * it is used in conjunction with NXP Semiconductors microcontrollers.  This
 * copyright, permission, and disclaimer notice must appear in all copies of
 * this code.
 */

#include "I2C.h"

I2C::I2C (const I2C_config &cfg) : device (nullptr)
{
  // if(cfg.device_number == 0) {
  this->device = LPC_I2C0;

  /*
    Pins
    The pin requires an external pull-up to provide output functionality.
    When power is switched off, this pin is floating and does not disturb the I2C lines.

    I2C0_SCL (0, 4).                   (Available for Fast Mode Plus)
    I2C1_SCL (0, 7), (1, 11), (1, 30). (Not open-drain)
    I2C0_SDA (0, 5).                   (Available for Fast Mode Plus)
    I2C1_SDA (1, 3), (1, 14), (1, 24). (Not open-drain)
  */
  //Manual: Table 83 & 90
  Chip_SYSCTL_PeriphReset(RESET_I2C0);
  Chip_IOCON_PinMuxSet(LPC_IOCON, ThreadCommon::PORT_I2C_SCL, 
                      ThreadCommon::PIN_I2C_SCL, IOCON_FUNC1 |
                       IOCON_DIGMODE_EN | cfg.i2c_mode);
  Chip_IOCON_PinMuxSet(LPC_IOCON, ThreadCommon::PORT_I2C_SDA, 
                      ThreadCommon::PIN_I2C_SDA, IOCON_FUNC1 |
                       IOCON_DIGMODE_EN | cfg.i2c_mode);
  //}
  // else {
  // currently we support only I2C number 0
  //}

  if (LPC_I2C0)
    {
      /* Enable I2C clock and reset I2C peripheral - the boot ROM does not
 do this */
      Chip_I2CM_Init(this->device);

      /* Setup clock rate for I2C */
      //No clock divider requiered?

      /* Setup I2CM transfer rate */
      //Bus speed (Determines required amount of clockcyckles for LOW and HIGH signals itself)
      Chip_I2CM_SetBusSpeed(this->device, cfg.speed);

      /* Enable Master Mode */
      //Enabled by Chip_I2CM_SendStart()
    }
}

I2C::~I2C () {}

bool
I2C::write (uint8_t devAddr, uint8_t *txBuffPtr, uint16_t txSize)
{
  return transaction (devAddr, txBuffPtr, txSize, nullptr, 0);
}

bool
I2C::read (uint8_t devAddr, uint8_t *rxBuffPtr, uint16_t rxSize)
{
  return transaction (devAddr, nullptr, 0, rxBuffPtr, rxSize);
}

bool
I2C::transaction (uint8_t devAddr, uint8_t *txBuffPtr, uint16_t txSize,
                  uint8_t *rxBuffPtr, uint16_t rxSize)
{
  I2CM_XFER_T i2cmXferRec;

  // make sure that master is idle
  //while (Chip_I2CM_StateChanged(this->device) == 0);

  /* Setup I2C transfer record */
  i2cmXferRec.slaveAddr = devAddr;
  i2cmXferRec.status = 0;
  i2cmXferRec.txSz = txSize;
  i2cmXferRec.rxSz = rxSize;
  i2cmXferRec.txBuff = txBuffPtr;
  i2cmXferRec.rxBuff = rxBuffPtr;

  I2CM_XferBlocking (this->device, &i2cmXferRec);
  // Chip_I2CM_XferBlocking returns before stop condition is fully completed
  // therefore we need to wait for master to be idle when doing back-to-back
  // transactions (see beginning of the function)

  /* Test for valid operation */
  if (i2cmXferRec.status == I2CM_STATUS_OK)
    {
      return true;
    }
  else
    {
      return false;
    }
}

/* Transmit and Receive data in master mode */
/* This duplicates (and combines) the functionality of Chip_I2CM_Xfer and
 * Chip_I2CM_XferBlocking with a modification that allows us to do a zero
 * length write (needed to use honeywell humidity/temp sensor)
 */
uint32_t
I2C::I2CM_XferBlocking (LPC_I2C_T *pI2C, I2CM_XFER_T *xfer)
{
  uint32_t ret = 0;
  /* start transfer */

  /* set the transfer status as busy */
  xfer->status = I2CM_STATUS_BUSY;
  /* Clear controller state. */
	Chip_I2CM_ResetControl(pI2C);

  /* Write Address and RW bit to data register */
  // Chip_I2CM_WriteByte(pI2C, (xfer->slaveAddr << 1) | (xfer->txSz == 0)); //
  // original NXP version
  // krl : both read and write lenght is 0 --> write (for honeywell temp
  // sensor)
  Chip_I2CM_WriteByte (pI2C, (xfer->slaveAddr << 1)
                                 | (xfer->txSz == 0 && xfer->rxSz != 0));
  /* Enter to Master Transmitter mode */
  Chip_I2CM_SendStart (pI2C);

  while (ret == 0)
    {
      /* wait for status change interrupt */
         while (Chip_I2CM_StateChanged(pI2C) == 0)
        {
        }
      /* call state change handler */
      ret = Chip_I2CM_XferHandler (pI2C, xfer);
    }
  return ret;
}
