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

	AT24CxEeprom(TwoWire& wire, uint8_t deviceAddress /* 0..7 */);

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
	virtual uint32_t pageSize() const = 0;

private:
	enum ERROR : uint8_t {
		WIRE_NO_ERROR = 0,
		WIRE_ADDR_TRANSMISSION_NACK = 2,
		WIRE_DATA_TRANSMISSION_NACK = 3,
		WIRE_FINISH_UP_ERR = 4,

		// This is a At24C256eeprom specific ERROR
		NO_DATA_AVAILABLE,
	};

	inline bool isNoError(const ERROR error)const {
	  return (error == WIRE_NO_ERROR);
	}

	uint8_t mAT24CxDeviceAddress;
	TwoWire& mWire;

	inline uint32_t pageOffsetMask()const {return pageSize()-1;}
	inline uint32_t pageMask()const {return ~pageOffsetMask();}

	ERROR writeToPage(const uint16_t pageAlignedAddress, const uint8_t pageOffset,
		const uint8_t* bytes, const size_t count);

	ERROR readFromPage(const uint16_t pageAlignedAddress, const uint8_t pageOffset,
		uint8_t* bytes, const size_t count);

	// This limits the number of bytes that are read in one read operation. It can
	// be overridden by a user defined AT24C - class.
	virtual size_t maxBulkReadQuantity() const;
};

class AT24C01 : public AT24CxEeprom { // 1 KBit
public:
  AT24C01(TwoWire &wire, uint8_t deviceAddress);
  uint32_t totalSize() const override;
  uint32_t pageSize() const override;
};

class AT24C02 : public AT24CxEeprom { // 2 KBit
public:
  AT24C02(TwoWire &wire, uint8_t deviceAddress);
  uint32_t totalSize() const override;
  uint32_t pageSize() const override;
};

class AT24C04 : public AT24CxEeprom { // 4 KBit
public:
  AT24C04(TwoWire &wire, uint8_t deviceAddress);
  uint32_t totalSize() const override;
  uint32_t pageSize() const override;
};

class AT24C08 : public AT24CxEeprom { // 8 KBit
public:
  AT24C08(TwoWire &wire, uint8_t deviceAddress);
  uint32_t totalSize() const override;
  uint32_t pageSize() const override;
};

class AT24C16 : public AT24CxEeprom { // 16 KBit
public:
  AT24C16(TwoWire &wire, uint8_t deviceAddress);
  uint32_t totalSize() const override;
  uint32_t pageSize() const override;
};

class AT24C32 : public AT24CxEeprom { // 32 KBit
public:
	AT24C32(TwoWire &wire, uint8_t deviceAddress);
	uint32_t totalSize() const override;
	uint32_t pageSize() const override;
};

class AT24C64 : public AT24CxEeprom { // 64 KBit
public:
	AT24C64(TwoWire &wire, uint8_t deviceAddress);
	uint32_t totalSize() const override;
private:
	uint32_t pageSize() const override;
};

class AT24C128 : public AT24CxEeprom { // 128 KBit
public:
	AT24C128(TwoWire &wire, uint8_t deviceAddress);
	uint32_t totalSize() const override;
	uint32_t pageSize() const override;
};

class AT24C256 : public AT24CxEeprom { // 256 KBit
public:
	AT24C256(TwoWire &wire, uint8_t deviceAddress);
	uint32_t totalSize() const override;
	uint32_t pageSize() const override;
};

class AT24C512 : public AT24CxEeprom { // 512 KBit
public:
	AT24C512(TwoWire &wire, uint8_t deviceAddress);
	uint32_t totalSize() const override;
	uint32_t pageSize() const override;
};

class AT24CM01 : public AT24CxEeprom { // 1 Mbit
public:
  AT24CM01(TwoWire &wire, uint8_t deviceAddress);
  uint32_t totalSize() const override;
  uint32_t pageSize() const override;
};

class AT24CM02 : public AT24CxEeprom { // 2 Mbit
public:
  AT24CM02(TwoWire &wire, uint8_t deviceAddress);
  uint32_t totalSize() const override;
  uint32_t pageSize() const override;
};

#endif /* AT24Cx_HPP_ */
