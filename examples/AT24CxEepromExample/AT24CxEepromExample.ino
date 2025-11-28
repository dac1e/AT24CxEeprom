#include "Arduino.h"

#include "AT24CxEeprom.h"

static AT24C512 eeprom(Wire, 0);
//static AT24C256 eeprom(Wire, 0);
//static AT24C128 eeprom(Wire, 0);
//static AT24C64 eeprom(Wire, 0);
//static AT24C32 eeprom(Wire, 0);

static typeof(Serial) output = Serial;
//static typeof(Serial1) output = Serial1;

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

void testWriteRead(uint8_t pattern) {
  const size_t count = eeprom.pageSize();
  const uint16_t eepromAddress = 0x08;

  output.print("Write and read ");
  output.print(count);
  output.print(" times pattern ");
  output.print(pattern, HEX);
  output.print("at address ");
  output.println(eepromAddress, HEX);

  // Write to and read back from eeprom adress 1
  const bool bOk = writeReadAndCompare(eeprom.pageSize(), pattern, eepromAddress);

  if(bOk) {
    output.println("Pass!");
  } else {
    output.println("Fail!");
  }
}

//The setup function is called once at startup of the sketch
void setup()
{
  output.begin(9600);
  output.println("AT24CxEeprom demo");
  output.flush();

  testWriteRead(0x55);
  testWriteRead(0xaa);
}

// The loop function is called in an endless loop
void loop()
{
//Add your repeated code here
}
