/*
  AT24eeprom - Arduino libary for driving the AT24 I2C based eeproms Copyright (c)
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

#ifndef AT24CxEepromEnableTest
#define AT24CxEepromEnableTest false
#endif

#if AT24CxEepromEnableTest

#include <Arduino.h>
#include <Wire.h>

#include "../AT24CxEeprom.h"
#include "AT24CxTest.h"

#define UTS_BEGIN() printTestFunctionEntry(__FUNCTION__)
#define UTS_END()   printTestFunctionExit (__FUNCTION__)

namespace AT24CxTest {

Test Test::instance;

namespace { // anonymous

void fillBuffer(uint8_t* buffer, size_t count, uint8_t pattern) {
	for(size_t i=0; i< count; i++) {
		buffer[i] = pattern;
	}
}

}

bool Test::writeReadAndCompare(size_t bytesCount, uint8_t pattern, uint16_t address) {
	uint8_t* writeBuffer = new uint8_t [2*mEeprom.pageSize()];

	fillBuffer(writeBuffer, bytesCount, pattern);
	writeBuffer[0] = ~pattern; // First byte becomes different
	writeBuffer[bytesCount-1] = ~pattern; // Last byte becomes different
	mEeprom.write(address, writeBuffer , bytesCount);


	delay(1);

	uint8_t* readBuffer = new uint8_t [2*mEeprom.pageSize()];
	mEeprom.read(address,readBuffer, bytesCount);

	const bool result = ( memcmp(readBuffer, writeBuffer, bytesCount) == 0 );

	delete[] readBuffer;
	delete[] writeBuffer;

	return result;
}


Test::Test()
		: base_t(Serial), mEeprom(Wire, 0) {
}

void Test::setup() {
	mEeprom.begin();
}

void Test::test_byteOperations() {
	UTS_BEGIN();

	const uint16_t addr = 0;

	uint8_t byte0;
	mEeprom.read(addr, byte0);
	byte0 = ~byte0;

	mEeprom.write(addr, byte0);

	uint8_t byte1;
	mEeprom.read(addr, byte1);

	utsAssert(byte1 == byte0);

	UTS_END();
}

void Test::test_pageOperations() {
	UTS_BEGIN();

	// Write 1 time page size
	utsAssert(writeReadAndCompare(mEeprom.pageSize() / 2, 0x55, 0)); // All bytes within one pages.
	utsAssert(writeReadAndCompare(mEeprom.pageSize(), 0x11, 1)); // Last byte is on 2nd page.
	utsAssert(writeReadAndCompare(mEeprom.pageSize(), 0x22, mEeprom.pageSize()-1)); // All but first byte is on 2nd page.

	// Write 2 times page size
	utsAssert(writeReadAndCompare(2*mEeprom.pageSize(), 0x33, 0)); // All bytes within 2 pages.
	utsAssert(writeReadAndCompare(2*mEeprom.pageSize(), 0x44, 1)); // Last byte on 3rd page.
	utsAssert(writeReadAndCompare(2*mEeprom.pageSize(), 0x55, mEeprom.pageSize())); // All but first byte on 2nd and 3rd page.

	UTS_END();
}

} // namespace At24C256test

#endif // AT24CxEepromEnableTest


