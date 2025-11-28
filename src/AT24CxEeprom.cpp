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

#include <algorithm>
#include <stdint.h>
#include <assert.h>
#define ASSERT assert

#include <Arduino.h>
#include <Wire.h>

#include "AT24CxEeprom.h"
#include "test\AT24CxDebug.h"

// Set this macro to true for printing debug output.
#ifndef AT24_ENABLE_DEBUG
#define AT24_ENABLE_DEBUG true
#endif


#ifdef ARDUINO_ARCH_SAM
	#include <WireBuffer.h>
	constexpr size_t RECEIVE_BUFFER_SIZE  = 32;
	constexpr size_t TRANSMIT_BUFFER_SIZE = 34;

	SET_Wire_BUFFERS(RECEIVE_BUFFER_SIZE, TRANSMIT_BUFFER_SIZE,
		true /* master buffers needed */, false /* no slave buffers needed */ );
#endif

#undef min
#undef max


// Map AT24 debug macros to Debug class
#define debugAt24_println 					AT24CxDebug::Debug<AT24CxDebug::AT24_, AT24_ENABLE_DEBUG>::println
#define debugAt24_print	 						AT24CxDebug::Debug<AT24CxDebug::AT24_, AT24_ENABLE_DEBUG>::print
#define debugAt24_printts 					AT24CxDebug::Debug<AT24CxDebug::AT24_, AT24_ENABLE_DEBUG>::printts
#define debugAt24_printtsln 				AT24CxDebug::Debug<AT24CxDebug::AT24_, AT24_ENABLE_DEBUG>::printtsln

void AT24CxEeprom::begin() {
#if AT24_ENABLE_DEBUG
  AT24_DEBUG_OUTPUT.begin(115200);
#endif
	mWire.begin();
}

void AT24CxEeprom::begin(CLOCK_SPEED_HZ speed) {
	begin();
	mWire.setClock(speed);
}

bool AT24CxEeprom::write(const uint16_t address, const uint8_t byte) {
	debugAt24_printts("write, at address ");
	debugAt24_println(address);

	ASSERT(address < totalSize());

	size_t i = 0;
	while (i < WRITE_RETRIES) {
		mWire.beginTransmission(mAT24CxDeviceAddress);

		// write address
		mWire.write(highByte(address));
		mWire.write(lowByte(address));

		// write data
		mWire.write(byte);

		const ERROR error = static_cast<ERROR>(mWire.endTransmission());

		debugAt24_print("\tmWire.endTransmission returned error ");
		debugAt24_print(error);
		debugAt24_print(", in try: ");
		debugAt24_println(i);

		if (isOk(error)) {
			return true;
		}

		++i;
		delay(1);
	}

	return false;
}

bool AT24CxEeprom::read(const uint16_t address, uint8_t &byte) {
	debugAt24_printts("read, at address ");
	debugAt24_println(address);

	size_t i = 0;
	while (i < READ_RETRIES) {
		mWire.beginTransmission(mAT24CxDeviceAddress);

		// write address
		mWire.write(highByte(address));
		mWire.write(lowByte(address));

		const ERROR error = static_cast<ERROR>(mWire.endTransmission());

		debugAt24_print("\tmWire.endTransmission returned error ");
		debugAt24_print(error);
		debugAt24_print(", in try: ");
		debugAt24_print(i);

		if (isOk(error)) {
			mWire.requestFrom(mAT24CxDeviceAddress, static_cast<uint8_t>(1));
			if (mWire.available()) {
				byte = mWire.read();
				debugAt24_print(", byte: ");
				debugAt24_println(byte);
			} else {
				debugAt24_println(", no byte available");
			}
			return true;
		} else {
			debugAt24_println();
		}
		++i;
		delay(1);
	}
	return false;
}

bool AT24CxEeprom::write(const uint16_t address, const uint8_t *bytes, const size_t count) {
	uint16_t pageAlignedAddress = address & pageMask();
	uint8_t pageOffset = address & pageOffsetMask();

	size_t i = 0;
	size_t n = std::min(count, size_t(pageSize()) - static_cast<size_t>(pageOffset));

	ERROR error = WIRE_NO_ERROR;
	while (((count - i) > 0) && isOk(error)) {
		error = writeToPage(pageAlignedAddress, pageOffset, &bytes[i], n);
		pageAlignedAddress += pageSize();
		pageOffset = 0;
		i += n;
		n = std::min((count - i), size_t(pageSize()));
	}
	return isOk(error);
}

AT24CxEeprom::ERROR AT24CxEeprom::writeToPage(const uint16_t pageAlignedAddress, const uint8_t pageOffset,
		const uint8_t *bytes, const size_t count) {

	debugAt24_printts("writeToPage, at address ");
	debugAt24_println(pageAlignedAddress + pageOffset);

	ASSERT((pageAlignedAddress & pageOffsetMask()) == 0);
	ASSERT((static_cast<uint32_t>(pageOffset) + count) <= pageSize());
	ASSERT((static_cast<uint32_t>(pageAlignedAddress) + pageSize()) <= totalSize());

	ERROR error = WIRE_NO_ERROR;

	size_t bytesWritten = 0;
	while (((count - bytesWritten)) > 0 && isOk(error)) {
		size_t n = 0;
		size_t w = 0;
		while (w < WRITE_RETRIES) {
			mWire.beginTransmission(mAT24CxDeviceAddress);

			// write address
			const uint16_t address = pageAlignedAddress + pageOffset + bytesWritten;
			mWire.write(highByte(address));
			mWire.write(lowByte(address));

			// write data
			n = mWire.write(&bytes[bytesWritten], count - bytesWritten);
			error = static_cast<ERROR>(mWire.endTransmission());

			debugAt24_print("\tmWire.endTransmission returned error ");
			debugAt24_print(error);
			debugAt24_print(", in try: ");
			debugAt24_print(w);

			if (isOk(error)) {
				debugAt24_print(", bytesWritten: ");
				debugAt24_println(bytesWritten + n);
				break;
			} else {
				debugAt24_println();
			}

			++w;
			delay(1);
		}

		bytesWritten += n;
		debugAt24_printts("write (re)tries: ");
		debugAt24_println(w);
	}

	return error;
}

AT24CxEeprom::ERROR AT24CxEeprom::readFromPage(const uint16_t pageAlignedAddress, const uint8_t pageOffset,
		uint8_t *bytes, const size_t count) {

	debugAt24_printts("readFromPage, at address ");
	debugAt24_println(pageAlignedAddress + pageOffset);

	ASSERT((pageAlignedAddress & pageOffsetMask()) == 0);
	ASSERT((static_cast<uint32_t>(pageOffset) + count) <= pageSize());
	ASSERT((static_cast<uint32_t>(pageAlignedAddress) + pageSize()) <= totalSize());

	ERROR error = WIRE_NO_ERROR;

	size_t bytesRead = 0;
	while (((count - bytesRead) > 0) && isOk(error)) {
		size_t n = 0;
		size_t r = 0;
		while (r < READ_RETRIES) {
			mWire.beginTransmission(mAT24CxDeviceAddress);

			// write address
			const uint16_t address = pageAlignedAddress + pageOffset + bytesRead;
			mWire.write(highByte(address));
			mWire.write(lowByte(address));
			error = static_cast<ERROR>(mWire.endTransmission());

			debugAt24_print("\tmWire.endTransmission returned error ");
			debugAt24_print(error);
			debugAt24_print(", in try: ");
			debugAt24_print(r);

			if (isOk(error)) {
				const size_t i = bytesRead;

				// There is a bug in the Wire library that will cause an out of bounds crash, if we
				// request more than the SERIAL_BUFFER_SIZE. Hence we better do the limitation here.
				const size_t nrequest = std::min(static_cast<size_t>(SERIAL_BUFFER_SIZE), count - bytesRead);
				n = mWire.requestFrom(mAT24CxDeviceAddress, nrequest);

				if (mWire.available()) {
					debugAt24_print(", bytesRead: ");
					debugAt24_println(bytesRead + n);

					for (size_t j = 0; j < n; j++) {
						const int data = mWire.read();
						ASSERT(data >= 0);
						bytes[i + j] = lowByte(data);
					}
				} else {
					error = NO_DATA_AVAILABLE;
					debugAt24_println(", no data available");
				}

				break;
			} else {
				debugAt24_println();
			}

			++r;
			delay(1);
		}
		bytesRead += n;

		debugAt24_printts("read (re)tries: ");
		debugAt24_println(r);
	}

	return error;
}

bool AT24CxEeprom::read(const uint16_t address, uint8_t *bytes, const size_t count) {
	uint16_t pageAlignedAddress = address & pageMask();
	uint8_t pageOffset = address & pageOffsetMask();

	uint32_t i = 0;
	uint32_t n = std::min(count, size_t(pageSize()) - static_cast<size_t>(pageOffset));

	ERROR error = WIRE_NO_ERROR;
	while (((count - i) > 0) && isOk(error)) {
		error = readFromPage(pageAlignedAddress, pageOffset, &bytes[i], n);
		pageAlignedAddress += pageSize();
		pageOffset = 0;
		i += n;
		n = std::min((count - i), uint32_t(pageSize()));
	}
	return isOk(error);
}

AT24CxEeprom::AT24CxEeprom(TwoWire& wire, uint8_t deviceAddress)
		: mAT24CxDeviceAddress((deviceAddress | 0xa0) >> 1), mWire(wire) {
}

// --- Specific chips
AT24C32::AT24C32(TwoWire &wire, uint8_t deviceAddress)
		: AT24CxEeprom(wire, deviceAddress) {
}

uint32_t AT24C32::totalSize() const {
	return 0x1000;
}

uint32_t AT24C32::pageSize() const {
	return 32;
}

AT24C64::AT24C64(TwoWire &wire, uint8_t deviceAddress)
		: AT24CxEeprom(wire, deviceAddress) {
}

uint32_t AT24C64::totalSize() const {
	return 0x2000;
}

uint32_t AT24C64::pageSize() const {
	return 32;
}

AT24C128::AT24C128(TwoWire &wire, uint8_t deviceAddress)
		: AT24CxEeprom(wire, deviceAddress) {
}

uint32_t AT24C128::totalSize() const {
	return 0x4000;
}

uint32_t AT24C128::pageSize() const {
	return 64;
}

AT24C256::AT24C256(TwoWire &wire, uint8_t deviceAddress)
		: AT24CxEeprom(wire, deviceAddress) {
}

uint32_t AT24C256::totalSize() const {
	return 0x8000;
}

uint32_t AT24C256::pageSize() const {
	return 64;
}

AT24C512::AT24C512(TwoWire &wire, uint8_t deviceAddress)
		: AT24CxEeprom(wire, deviceAddress) {
}

uint32_t AT24C512::totalSize() const {
	return 0x10000;
}

uint32_t AT24C512::pageSize() const {
	return 128;
}

