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

#pragma once

#ifndef AT24Cx_HPP_
#define AT24Cx_HPP_

#include <stdint.h>
#include <stddef.h>
#include <Wire.h>

class AT24CxEeprom {
public:
	enum CLOCK_SPEED_HZ {
		CLK_STANDARD_SPEED = 100000,
		CLK_HIGH_SPEED = 400000,
	};

	/**
	 * Constructor
	 * @param The Wire interface object to be used for I2C communication.
	 * @param The lower 3 bits A2, A1, A0 of the device address 0..7
	 */
	AT24CxEeprom(TwoWire& wire, uint8_t deviceAddress);

	/**
	 * Initialize I2C bus for communication with EEPROM
	 * To be called before any read write operation.
	 * May be skipped if the I2C bus is already initialized.
	 */
	void begin();

	/**
	 * Initialize I2C bus for communication with EEPROM
	 * To be called before any read write operation.
	 * May be skipped if the I2C bus is already initialized.
	 * @param speed The frequency that shall be used for the I2C bus communication.
	 */
	void begin(CLOCK_SPEED_HZ speed);

	/**
	 * Write a single byte.
	 * @param address eeprom address where the byte shall be written to.
	 * @param byte the byte that shall be written.
	 * @return true, on success, otherwise false.
	 */
	bool write(const uint16_t address, const uint8_t byte);

	/**
	 * Write multiple bytes.
	 * @param address eeprom address where the first byte shall be written to.
	 * @param bytes the bytes that shall be written.
	 * @return true, on success, otherwise false.
	 */
	bool write(const uint16_t address, const uint8_t* bytes, const size_t count);

	/**
	 * Read a single byte.
	 * @param address eeprom address from where the byte shall be read.
	 * @param byte the location where the read byte shall be returned.
	 * @return true, on success, otherwise false.
	 */
	bool read(const uint16_t address, uint8_t& byte);

	/**
	 * Read a multiple bytes.
	 * @param address eeprom address from where the first byte shall be read.
	 * @param bytes the location where the read bytes shall be returned.
	 * @return true, on success, otherwise false.
	 */
	bool read(const uint16_t address, uint8_t* bytes, const size_t count);

	/**
	 * get the total size of the eeprom.
	 * @return the total size of the eeprom in bytes.
	 */
	virtual uint32_t totalSize() const = 0;

	/**
	 * get the page size of the eeprom.
	 * @return the page size of the eeprom in bytes.
	 */
	virtual uint32_t pageSize()const = 0;

private:
	enum ERROR : uint8_t {
		WIRE_NO_ERROR = 0,
		WIRE_ADDR_TRANSMISSION_NACK = 2,
		WIRE_DATA_TRANSMISSION_NACK = 3,
		WIRE_FINISH_UP_ERR = 4,

		// This is a At24C256eeprom specific ERROR
		NO_DATA_AVAILABLE,
	};

	inline bool isOk(const ERROR error)const {return (error == WIRE_NO_ERROR);}

	static constexpr size_t WRITE_RETRIES = 8;
	static constexpr size_t READ_RETRIES  = 8;

	uint8_t mAT24CxDeviceAddress;
	TwoWire& mWire;

	inline uint32_t pageOffsetMask()const {return pageSize()-1;}
	inline uint32_t pageMask()const {return ~pageOffsetMask();}

	ERROR writeToPage(const uint16_t pageAlignedAddress, const uint8_t pageOffset,
		const uint8_t* bytes, const size_t count);

	ERROR readFromPage(const uint16_t pageAlignedAddress, const uint8_t pageOffset,
		uint8_t* bytes, const size_t count);
};

class AT24C32 : public AT24CxEeprom {
public:
	AT24C32(TwoWire &wire, uint8_t deviceAddress);
	uint32_t totalSize() const override;
	uint32_t pageSize() const override;
};

class AT24C64 : public AT24CxEeprom {
public:
	AT24C64(TwoWire &wire, uint8_t deviceAddress);
	uint32_t totalSize() const override;
private:
	uint32_t pageSize() const override;
};

class AT24C128 : public AT24CxEeprom {
public:
	AT24C128(TwoWire &wire, uint8_t deviceAddress);
	uint32_t totalSize() const override;
	uint32_t pageSize() const override;
};

class AT24C256 : public AT24CxEeprom {
public:
	AT24C256(TwoWire &wire, uint8_t deviceAddress);
	uint32_t totalSize() const override;
	uint32_t pageSize() const override;
};

class AT24C512 : public AT24CxEeprom {
public:
	AT24C512(TwoWire &wire, uint8_t deviceAddress);
	uint32_t totalSize() const override;
	uint32_t pageSize() const override;
};

#endif /* AT24Cx_HPP_ */
