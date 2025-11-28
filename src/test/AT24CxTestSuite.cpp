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

#include <Arduino.h>
#include <assert.h>
#define ASSERT assert

#undef min
#undef max

#include "AT24CxTestSuite.h"

namespace AT24CxTest {

TestSuite::TestSuite(Print& testLogOutput) : mTestLogOutput(testLogOutput) {
}

void TestSuite::printTestFunction(const char* const functionName) {
	mTestLogOutput.print(suiteName());
	mTestLogOutput.print("::");
	mTestLogOutput.print(functionName);
	mTestLogOutput.println(" ********");
}

void TestSuite::printTestFunctionEntry(const char* const functionName) {
	mTestLogOutput.print("\n******** entering ");
	printTestFunction(functionName);
}

void TestSuite::printTestFunctionExit(const char* const functionName) {
	mTestLogOutput.print("******** exiting ");
	printTestFunction(functionName);
}

void TestSuite::utsAssert(bool expression) {
	if(!expression) {
		mTestLogOutput.print("test failed");
		ASSERT(false);
	}
}

void TestSuite::utsmAssert(bool expression, const char* const failureMessage) {
	if(!expression) {
		mTestLogOutput.print(failureMessage);
		ASSERT(false);
	}
}

} // namespace At24C256test
