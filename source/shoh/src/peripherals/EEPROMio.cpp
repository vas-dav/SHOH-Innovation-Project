/*
 * EEPROMio.cpp
 *
 *  Created on: Dec 4, 2022
 *      Author: tylen
 */

#include <EEPROMio.h>
#include "Log.h"

static void
e_memcpy (void *from, void *to, unsigned int n)
{
  if (!from)
    return;
  char *source = (char *)from;
  char *dest = (char *)to;
  while (n)
    {
      (*dest++) = (*source++);
      n--;
    }
}

void
EEPROMio::eeprom_execute (EEPROM *rom)
{
  command[0] = rom->mode;
  command[1] = rom->addr;
  command[2] = rom->data;
  command[3] = rom->size;
  command[4] = rom->clock;
  iap_entry(command, result);
}

void
EEPROMio::eeprom_use (uint8_t *data, uint32_t addr, uint32_t size,
                            bool mode)
{
  rom.addr = addr;
  rom.data = (uint32_t)data;
  rom.mode = (mode) ? IAP_EEPROM_READ : IAP_EEPROM_WRITE;
  rom.size = size;
  rom.clock = SystemCoreClock / 1000;
#if INCLUDE_vTaskSuspend
  vTaskSuspendAll ();
#endif
  /* Main execution of eeprom r/w */
  eeprom_execute (&rom);
#if INCLUDE_vTaskSuspend
  xTaskResumeAll ();
#endif
  assert (result[0] == IAP_CMD_SUCCESS);
}

void
EEPROMio::write_to (uint32_t addr, std::string str)
{
  std::copy (str.begin (), str.end (), std::begin (buffer));
  eeprom_use (buffer, addr, str.length (), WRITE);
  LOG_DEBUG("%dB written to EEPROM", str.length ());
}

void *
EEPROMio::read_from (uint32_t addr, uint32_t amount)
{
  eeprom_use (buffer, addr, amount, READ);
  void *data = (void *)buffer;
  LOG_DEBUG("%dB read from EEPROM", amount);
  return data;
}
void
EEPROMio::write_to (uint32_t addr, void *data, uint32_t size_of_data)
{
  assert (size_of_data < EEPROM_MAX_BUFER_SIZE);
  e_memcpy (data, buffer, size_of_data);
  eeprom_use (buffer, addr, size_of_data, WRITE);
  LOG_DEBUG("%dB written to EEPROM", size_of_data);
}

