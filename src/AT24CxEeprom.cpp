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




#include <stdint.h>
#include <assert.h>
#define ASSERT assert

#include <Arduino.h>
#include <Wire.h>

#include "AT24CxEeprom.h"

#undef min
#undef max

static inline size_t min(size_t a, size_t b) {return a < b ? a : b;}

#if defined SIZE_MAX
static size_t sizeMax() {return SIZE_MAX;}
#else
#include <limits>
static size_t sizeMax() {return std::numeric_limits<size_t>::max();}
#endif

static constexpr size_t WRITE_RETRIES = 10;
static constexpr size_t READ_RETRIES  = 10;

void AT24CxEeprom::begin() {
	mWire.begin();
}

void AT24CxEeprom::begin(CLOCK_SPEED_HZ speed) {
	begin();
	mWire.setClock(speed);
}

bool AT24CxEeprom::write(const uint16_t address, const uint8_t byte) {
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
		if (isNoError(error)) {
			return true;
		}

		++i;
		delay(1);
	}

	return false;
}

bool AT24CxEeprom::read(const uint16_t address, uint8_t &byte) {
	size_t i = 0;
	while (i < READ_RETRIES) {
		mWire.beginTransmission(mAT24CxDeviceAddress);

		// write address
		mWire.write(highByte(address));
		mWire.write(lowByte(address));

		const ERROR error = static_cast<ERROR>(mWire.endTransmission());

		if (isNoError(error)) {
			mWire.requestFrom(mAT24CxDeviceAddress, static_cast<uint8_t>(1));
			if (mWire.available()) {
				byte = mWire.read();
			}
			return true;
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
	size_t n = min(count, size_t(pageSize()) - static_cast<size_t>(pageOffset));

	ERROR error = WIRE_NO_ERROR;
	while (((count - i) > 0) && isNoError(error)) {
		error = writeToPage(pageAlignedAddress, pageOffset, &bytes[i], n);
		pageAlignedAddress += pageSize();
		pageOffset = 0;
		i += n;
		n = min((count - i), size_t(pageSize()));
	}
	return isNoError(error);
}

AT24CxEeprom::ERROR AT24CxEeprom::writeToPage(const uint16_t pageAlignedAddress, const uint8_t pageOffset,
		const uint8_t *bytes, const size_t count) {

	ASSERT((pageAlignedAddress & pageOffsetMask()) == 0);
	ASSERT((static_cast<uint32_t>(pageOffset) + count) <= pageSize());
	ASSERT((static_cast<uint32_t>(pageAlignedAddress) + pageSize()) <= totalSize());

	ERROR error = WIRE_NO_ERROR;

	size_t bytesWritten = 0;
	while (((count - bytesWritten)) > 0 && isNoError(error)) {
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

			if (isNoError(error)) {
				break;
			}

			++w;
			delay(1);
		}

		bytesWritten += n;
	}

	return error;
}

size_t AT24CxEeprom::maxBulkReadQuantity() const {
#if defined SERIAL_BUFFER_SIZE
  return static_cast<size_t>(SERIAL_BUFFER_SIZE);
#else
  return sizeMax();
#endif
}

AT24CxEeprom::ERROR AT24CxEeprom::readFromPage(const uint16_t pageAlignedAddress, const uint8_t pageOffset,
		uint8_t *bytes, const size_t count) {

	ASSERT((pageAlignedAddress & pageOffsetMask()) == 0);
	ASSERT((static_cast<uint32_t>(pageOffset) + count) <= pageSize());
	ASSERT((static_cast<uint32_t>(pageAlignedAddress) + pageSize()) <= totalSize());

	ERROR error = WIRE_NO_ERROR;

	size_t bytesRead = 0;
	while (((count - bytesRead) > 0) && isNoError(error)) {
		size_t n = 0;
		size_t r = 0;
		while (r < READ_RETRIES) {
			mWire.beginTransmission(mAT24CxDeviceAddress);

			// write address
			const uint16_t address = pageAlignedAddress + pageOffset + bytesRead;
			mWire.write(highByte(address));
			mWire.write(lowByte(address));
			error = static_cast<ERROR>(mWire.endTransmission());

			if (isNoError(error)) {
				const size_t i = bytesRead;

				const size_t quantity = min(maxBulkReadQuantity(), count - bytesRead);
				n = mWire.requestFrom(mAT24CxDeviceAddress, quantity);

				if (mWire.available()) {
					for (size_t j = 0; j < n; j++) {
						const int data = mWire.read();
						ASSERT(data >= 0);
						bytes[i + j] = lowByte(data);
					}
				} else {
					error = NO_DATA_AVAILABLE;
				}

				break;
			}

			++r;
			delay(1);
		}
		bytesRead += n;
	}

	return error;
}

bool AT24CxEeprom::read(const uint16_t address, uint8_t *bytes, const size_t count) {
	uint16_t pageAlignedAddress = address & pageMask();
	uint8_t pageOffset = address & pageOffsetMask();

	uint32_t i = 0;
	uint32_t n = min(count, size_t(pageSize()) - static_cast<size_t>(pageOffset));

	ERROR error = WIRE_NO_ERROR;
	while (((count - i) > 0) && isNoError(error)) {
		error = readFromPage(pageAlignedAddress, pageOffset, &bytes[i], n);
		pageAlignedAddress += pageSize();
		pageOffset = 0;
		i += n;
		n = min((count - i), uint32_t(pageSize()));
	}
	return isNoError(error);
}

AT24CxEeprom::AT24CxEeprom(TwoWire& wire, uint8_t deviceAddress)
		: mAT24CxDeviceAddress((deviceAddress & 0x07) | 0x50), mWire(wire) {
}

// --- Specific chips
AT24C01::AT24C01(TwoWire &wire, uint8_t deviceAddress)
    : AT24CxEeprom(wire, deviceAddress) {
}

uint32_t AT24C01::totalSize() const {
  return 0x80;
}

uint32_t AT24C01::pageSize() const {
  return 8;
}

AT24C02::AT24C02(TwoWire &wire, uint8_t deviceAddress)
    : AT24CxEeprom(wire, deviceAddress) {
}

uint32_t AT24C02::totalSize() const {
  return 0x100;
}

uint32_t AT24C02::pageSize() const {
  return 8;
}

AT24C04::AT24C04(TwoWire &wire, uint8_t deviceAddress)
    : AT24CxEeprom(wire, deviceAddress) {
}

uint32_t AT24C04::totalSize() const {
  return 0x200;
}

uint32_t AT24C04::pageSize() const {
  return 16;
}

AT24C08::AT24C08(TwoWire &wire, uint8_t deviceAddress)
    : AT24CxEeprom(wire, deviceAddress) {
}

uint32_t AT24C08::totalSize() const {
  return 0x400;
}

uint32_t AT24C08::pageSize() const {
  return 16;
}

AT24C16::AT24C16(TwoWire &wire, uint8_t deviceAddress)
    : AT24CxEeprom(wire, deviceAddress) {
}

uint32_t AT24C16::totalSize() const {
  return 0x800;
}

uint32_t AT24C16::pageSize() const {
  return 16;
}

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
