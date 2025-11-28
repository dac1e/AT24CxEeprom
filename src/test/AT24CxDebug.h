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

#include <stdint.h>
#include <stddef.h>

#ifndef AT24CxDEBUG_HPP_
#define AT24CxDEBUG_HPP_

// Set the debug output
#ifndef AT24_DEBUG_OUTPUT
#define AT24_DEBUG_OUTPUT Serial1
#endif

namespace AT24CxDebug {

enum DEBUG_DOMAIN {
	AT24_,
	DEBUG_DOMAIN_CNT
};

const char* const DEBUG_DOMAIN_TEXT[DEBUG_DOMAIN_CNT] = {
	"AT24    ",
};

// template class Debug with specialization to map print functions either
// to nothing (DEBUG_ENABLED=false) or Serial (DEBUG_ENABLED=false)

// class Debug<DEBUG_ENABLED> assumes DEBUG_ENABLED = false and maps static functions to nothing
template<enum DEBUG_DOMAIN, bool DEBUG_ENABLED> class Debug {
	static inline size_t printts() {return 0;}
public:
	// functions with no parameter
	static inline size_t println() {return 0;}

	// functions with single parameter
	template<typename X> static inline size_t println(const X&)  {return 0;}
	template<typename X> static inline size_t print(const X&)    {return 0;}
	template<typename X> static inline size_t printHex(const X&) {return 0;}
	template<typename X> static inline size_t printts(const X&)   {return 0;}
	template<typename X> static inline size_t printtsln(const X&) {return 0;}

	// functions with 2 parameters
	template<typename X, typename Y> static inline size_t println(const X&, const Y&)
		{return 0;}
	template<typename X, typename Y> static inline size_t print(const X&, const Y&)
		{return 0;}
};

class DebugBase {
protected:
	static Print* mSerial;
	// print time stamp
	static inline size_t printts() {
		if(mSerial) {
			size_t n = 0;
			n +=mSerial->print(millis());
			n += mSerial->print(": ");
			return n;
		}
		return 0;
	}
public:
	// functions with no parameter
	static inline size_t println() {if(mSerial) {return mSerial->println();} return 0;}

	// functions with single parameter
	template<typename X> static inline size_t println(const X& x)
		{if(mSerial) {return  mSerial->println(x);} return 0;}
	template<typename X> static inline size_t print(const X& x)
		{if(mSerial) {return  mSerial->print(x);} return 0;}

	// functions with 2 parameters
	template<typename X, typename Y> static inline size_t println(const X& x, const Y& y)
		{if(mSerial) {return mSerial->println(x,y);} return 0;}
	template<typename X, typename Y> static inline size_t print(const X& x, const Y& y)
		{if(mSerial) {return mSerial->print(x,y);} return 0;}
};

// class Debug<true> specialization maps static functions to mCom
template<enum DEBUG_DOMAIN dd> class Debug<dd, true> : public DebugBase {
public:
	// print with time stamp
	template<typename X> static inline size_t printts(const X& x) {
			if(mSerial) {
				size_t result = DebugBase::printts();
				result += mSerial->print(DEBUG_DOMAIN_TEXT[dd]);
				result += mSerial->print(" ");
				result += mSerial->print(x);
				return result;
			}
			return 0;
	}
	// print line with time stamp
	template<typename X> static inline size_t printtsln(const X& x) {
			if(mSerial) {
				size_t result = DebugBase::printts();
				result += mSerial->print(DEBUG_DOMAIN_TEXT[dd]);
				result += mSerial->print(" ");
				result += mSerial->println(x);
				return result;
			}
			return 0;
	}
};


} // AT24CxDebug namespace


#endif /* AT24CxDEBUG_HPP_ */
