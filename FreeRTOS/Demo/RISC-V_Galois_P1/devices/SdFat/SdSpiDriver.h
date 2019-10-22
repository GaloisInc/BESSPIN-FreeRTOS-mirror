/**
 * Copyright (c) 2011-2018 Bill Greiman
 * This file is part of the SdFat library for SD memory cards.
 *
 * MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */
/**
 * \file
 * \brief SpiDriver classes
 */
#ifndef SdSpiDriver_h
#define SdSpiDriver_h

#include <string.h>
#include "stdint.h"

//------------------------------------------------------------------------------
/**
 * \class SdSpiAltDriver
 * \brief Optimized SPI class for access to SD and SDHC flash memory cards.
 */
class SdSpiAltDriver {
 public:
  /** Activate SPI hardware. */
  void activate();
  /** Deactivate SPI hardware. */
  void deactivate();
  /** Initialize the SPI bus.
   *
   * \param[in] csPin SD card chip select pin.
   */
  void begin(uint8_t csPin);
  /** Receive a byte.
   *
   * \return The byte.
   */
  uint8_t receive();
  /** Receive multiple bytes.
  *
  * \param[out] buf Buffer to receive the data.
  * \param[in] n Number of bytes to receive.
  *
  * \return Zero for no error or nonzero error code.
  */
  uint8_t receive(uint8_t* buf, size_t n);
  /** Send a byte.
   *
   * \param[in] data Byte to send
   */
  void send(uint8_t data);
  /** Send multiple bytes.
   *
   * \param[in] buf Buffer for data to be sent.
   * \param[in] n Number of bytes to send.
   */
  void send(uint8_t* buf, size_t n);
  /** Set CS low. */
  void select();
  /** Set CS high. */
  void unselect();
 private:
};
//------------------------------------------------------------------------------
// Choose SPI driver for SdFat and SdFatEX classes.
/** SdFat uses custom fast SPI. */
typedef SdSpiAltDriver SdFatSpiDriver;
// Don't need virtual driver.
typedef SdFatSpiDriver SdSpiDriver;
#endif  // SdSpiDriver_h
