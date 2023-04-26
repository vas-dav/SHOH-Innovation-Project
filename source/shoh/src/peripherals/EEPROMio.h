/*
 * EEPROMio.h
 *
 *  Created on: Dec 4, 2022
 *      Author: tylen
 */

#ifndef EEPROMIO_H_
#define EEPROMIO_H_

#include "FreeRTOS.h"
#include "task.h"
#include "chip.h"
#include "board.h"
#include <assert.h>
#include <string>
#include <map>

#define READ true
#define WRITE false
#define EEPROM_MAX_BUFER_SIZE 1000
#define EEPROM_START_ADDR 0x00000100

class EEPROMio
{
public:
  /**
   * @brief Construct a new eeprom wrapper object
   *
   */
  EEPROMio ()
  {
    SystemCoreClockUpdate();
    SysTick_Config(SystemCoreClock / 10);
  }
  virtual ~EEPROMio () = default;
  /**
   * @brief Write a string to EEPROM
   *
   * @param addr - address to write on
   * @param str - string to write
   */
  void write_to (uint32_t addr, std::string str);
  /**
   * @brief Read data from EEPROM
   *
   * @param addr - address to read from
   * @param amount - amount of bytes to read
   * @return void* - data that was read
   */
  void *read_from (uint32_t addr, uint32_t amount);
  /**
   * @brief Write data to EEPROM
   *
   * @param addr - address to write on
   * @param data - data to be written
   * @param size_of_data - size of data to be written
   */
  void write_to (uint32_t addr, void *data, uint32_t size_of_data);

private:
  typedef struct _eeprom
  {
    uint32_t data;
    uint32_t addr;
    uint32_t size;
    uint32_t mode;
    uint32_t clock;
  } EEPROM;

  unsigned int command[5], result[4];
  EEPROM rom = { 0, 0, 0, 0, 72000 };
  void eeprom_execute (EEPROM *rom);
  void eeprom_use (uint8_t *data, uint32_t addr, uint32_t size, bool mode);
  uint8_t buffer[EEPROM_MAX_BUFER_SIZE] = { 0 };
};

#endif /* EEPROMIO_H_ */
