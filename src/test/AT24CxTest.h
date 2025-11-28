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

#pragma once

#ifndef AT24Cx_TEST_HPP_
#define AT24Cx_TEST_HPP_

#include <assert.h>
#define ASSERT assert

class AT24CxEeprom;

namespace AT24CxTest {

class Test {
public:
  static void run(AT24CxEeprom& eeprom) {
    instance.mEeprom = &eeprom;
    instance.setup();
    instance.test_byteOperations();
    instance.test_pageOperations();
    instance.mEeprom = nullptr;
  }

private:
	// The one and only at25eeprom
	AT24CxEeprom* mEeprom;

	Test(Print& testLogOutput);

	const char* suiteName() const {
		return "At24C256eeprom_test";

	}

	void setup();
	void test_pageOperations();
	void test_byteOperations();
	bool writeReadAndCompare(size_t bytesCount, uint8_t pattern, uint16_t address);

  Print& mTestLogOutput;

  void printTestFunctionEntry(const char *const functionName);
  void printTestFunctionExit(const char *const functionName);
  void printTestFunction(const char *const functionName);

  void utsAssert(bool expression);
  void utsmAssert(bool expression, const char *const failureMessage);

  template<typename T>
  void utsmAssert(bool expression, const char *const failureProlog, const T &value, const char *const failureEpilog);

	static Test instance;
};

template<typename T>
inline void Test::utsmAssert(bool expression, const char *const failureProlog, const T &value,
  const char *const failureEpilog) {
  if (!expression) {
    if (failureProlog) {
      mTestLogOutput.print(failureProlog);
    }
    mTestLogOutput.print(value);
    if (failureEpilog) {
      mTestLogOutput.print(failureEpilog);
    }
    ASSERT(false);
  }
}

} // namespace AT24CxTest

#endif /* AT24Cx_TEST_HPP_ */
