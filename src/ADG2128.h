/*
File:   ADG2128.h
Author: J. Ian Lindsay
Date:   2014.03.10


Copyright (C) 2014 J. Ian Lindsay
All rights reserved.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

*/

#ifndef ADG2128_CROSSPOINT_H
#define ADG2128_CROSSPOINT_H

// If debugging is enabled in the build, another dependency will be needed.
// https://github.com/jspark311/CppPotpourri
#define ADG2128_DEBUG 1

#include <inttypes.h>
#include <stdlib.h>
#include <Wire.h>
#ifdef ARDUINO
  #include "Arduino.h"
#else
#endif

#if defined(ADG2128_DEBUG)
  // If debugging is enabled in the build, another dependency will be needed.
  // https://github.com/jspark311/CppPotpourri
  #include <StringBuilder.h>
#endif  // ADG2128_DEBUG


#define ADG2128_DEFAULT_I2C_ADDR    0x70
#define ADG2128_SERIALIZE_VERSION   0x01
#define ADG2128_SERIALIZE_SIZE        17

/* Class flags. */
#define ADG2128_FLAG_INITIALIZED    0x0001
#define ADG2128_FLAG_ALLOW_MR_TO_C  0x0002
#define ADG2128_FLAG_ALLOW_R_TO_MC  0x0004
#define ADG2128_FLAG_PRESERVE_STATE 0x0008
#define ADG2128_FLAG_PINS_CONFD     0x0010
#define ADG2128_FLAG_FROM_BLOB      0x0020

#define ADG2128_FLAG_SERIAL_MASK    0x000E  // Only these bits are serialized.


enum class ADG2128_ERROR : int8_t {
  NO_ERROR           = 0,   // There was no error.
  ABSENT             = -1,  // The ADG2128 appears to not be connected to the bus.
  BUS                = -2,  // Something went wrong with the i2c bus.
  BAD_COLUMN         = -3,  // Column was out-of-bounds.
  BAD_ROW            = -4   // Row was out-of-bounds.
};


/*
* This class represents an Analog Devices ADG2128 8x12 analog cross-point switch. This switch is controlled via i2c.
* The 8-pin group are the columns, and the 12-pin group are rows.
*/
class ADG2128 {
  public:
    ADG2128(const uint8_t addr, const uint8_t rst_pin);
    ADG2128(const uint8_t* buf, const unsigned int len);
    ~ADG2128();

    #if defined(ADG2128_DEBUG)
      void printDebug(StringBuilder*);
    #endif  // ADG2128_DEBUG

    inline ADG2128_ERROR init() {  return init(_bus);  };
    ADG2128_ERROR init(TwoWire*); // Perform bus-related init tasks.
    ADG2128_ERROR reset();     // Resets the entire device.
    ADG2128_ERROR refresh();   // Forces a shadow refresh from hardware.

    /* Functions for manipulating individual switches. */
    ADG2128_ERROR changeRoute(uint8_t col, uint8_t row, bool sw_closed, bool defer);
    ADG2128_ERROR setRoute(uint8_t col, uint8_t row, bool defer = false);
    ADG2128_ERROR unsetRoute(uint8_t col, uint8_t row, bool defer = false);
    uint8_t serialize(uint8_t* buf, unsigned int len);
    int8_t  unserialize(const uint8_t* buf, const unsigned int len);
    uint8_t  getCols(uint8_t row);
    uint16_t getRows(uint8_t col);

    inline bool initialized() {  return _adg_flag(ADG2128_FLAG_INITIALIZED);  };
    inline bool preserveOnDestroy() {
      return _adg_flag(ADG2128_FLAG_PRESERVE_STATE);
    };
    inline void preserveOnDestroy(bool x) {
      _adg_set_flag(ADG2128_FLAG_PRESERVE_STATE, x);
    };

    static const char* const errorToStr(ADG2128_ERROR);


  private:
    const uint8_t _ADDR;       // The device address on the i2c bus
    const uint8_t _RESET_PIN;  // ADG2128 reset pin
    uint16_t _flags = 0;       // Class flags.
    TwoWire* _bus   = nullptr;
    uint8_t  _values[12];

    ADG2128_ERROR compose_first_byte(uint8_t col, uint8_t row, bool set, uint8_t* result);
    int8_t _ll_pin_init();
    int8_t _read_device();
    int8_t _write_device(uint8_t row, uint8_t conn);

    inline bool _from_blob() {   return _adg_flag(ADG2128_FLAG_FROM_BLOB); };

    /* Flag manipulation inlines */
    inline uint16_t _adg_flags() {                return _flags;           };
    inline bool _adg_flag(uint16_t _flag) {       return (_flags & _flag); };
    inline void _adg_clear_flag(uint16_t _flag) { _flags &= ~_flag;        };
    inline void _adg_set_flag(uint16_t _flag) {   _flags |= _flag;         };
    inline void _adg_set_flag(uint16_t _flag, bool nu) {
      if (nu) _flags |= _flag;
      else    _flags &= ~_flag;
    };
};

#endif    // ADG2128_CROSSPOINT_H
