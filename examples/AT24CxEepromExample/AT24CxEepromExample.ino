/*
  AT24eeprom - Arduino libary for driving the AT24 I2 based eeproms Copyright (c)
  2025 Wolfgang Schmieder.  All right reserved.

  Contributors:
  - Wolfgang Schmieder

  Project home: https://github.com/dac1e/AT24eeprom/

  This library is free software; you can redistribute it and/or modify it
  the terms of the GNU Lesser General Public License as under published
  by the Free Software Foundation; either version 3.0 of the License,
  or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
*/

#include "Arduino.h"

#include <Wire.h>
#include "AT24CxEeprom.h"

static AT24C512 eeprom(Wire, 0);
//static AT24C256 eeprom(Wire, 0);
//static AT24C128 eeprom(Wire, 0);
//static AT24C64 eeprom(Wire, 0);
//static AT24C32 eeprom(Wire, 0);

static typeof(Serial)& output = Serial;
//static typeof(Serial1)& output = Serial1;

static void fillBuffer(uint8_t* buffer, size_t bytesCount, uint8_t pattern) {
  for(size_t i=0; i< bytesCount; i++) {
    buffer[i] = pattern;
  }
}

static bool writeReadAndCompare(size_t bytesCount, uint8_t pattern, uint16_t eepromAddress) {
  uint8_t* writeBuffer = new uint8_t [bytesCount];

  fillBuffer(writeBuffer, bytesCount, pattern);
  eeprom.write(eepromAddress, writeBuffer , bytesCount);

  delay(1);

  uint8_t* readBuffer = new uint8_t [bytesCount];
  eeprom.read(eepromAddress,readBuffer, bytesCount);

  const bool result = (memcmp(readBuffer, writeBuffer, bytesCount) == 0);

  delete[] readBuffer;
  delete[] writeBuffer;

  return result;
}

static void testWriteRead(uint8_t pattern) {
  const size_t count = eeprom.pageSize();
  const uint16_t eepromAddress = 0x08;

  output.print("Write and read ");
  output.print(count);
  output.print(" times pattern ");
  output.print(pattern, HEX);
  output.print(" at address ");
  output.print(eepromAddress, HEX);
  output.print(" ... ");

  // Write to and read back from eeprom adress
  const bool bOk = writeReadAndCompare(eeprom.pageSize(), pattern, eepromAddress);

  if(bOk) {
    output.println(" Pass!");
  } else {
    output.println(" Fail!");
  }
}

static size_t constexpr LOOPS = 3;
static size_t nRemainingLoops = LOOPS;

//The setup function is called once at startup of the sketch
void setup()
{
  output.begin(115200);
  output.println();
  output.println("Starting write/read test.");
  eeprom.begin();
}


// The loop function is called in an endless loop
void loop()
{
  delay(3000);
  if(nRemainingLoops) {
    --nRemainingLoops;
    output.println();
    output.print("Executing test loop #");
    output.println(LOOPS - nRemainingLoops);
    testWriteRead(0x55);
    testWriteRead(0xAA);
    if(not nRemainingLoops) {
      output.println();
      output.println("Test finished.");
    }
  }
}
