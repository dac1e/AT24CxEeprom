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

#include "AT24CxTestSuite.h"

class AT24CxEeprom;

namespace AT24CxTest {

class Test : public TestSuite {
	typedef TestSuite base_t;

	// The one and only at25eeprom
	AT24CxEeprom* mEeprom;

	Test();

	const char* suiteName() const override {
		return "At24C256eeprom_test";

	}

	void setup();
	void test_pageOperations();
	void test_byteOperations();
	bool writeReadAndCompare(size_t bytesCount, uint8_t pattern, uint16_t address);

	static Test instance;

public:
	static void run(AT24CxEeprom& eeprom) {
		instance.mEeprom = &eeprom;
		instance.setup();
		instance.test_byteOperations();
		instance.test_pageOperations();
		instance.mEeprom = nullptr;
	}

};

} // namespace AT24CxTest

#endif /* AT24Cx_TEST_HPP_ */
