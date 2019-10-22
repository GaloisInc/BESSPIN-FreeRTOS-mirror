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
#include "SdSpiDriver.h"

extern "C" {
#include "spi.h"
}
//------------------------------------------------------------------------------
/** Initialize the SPI bus.
 *
 * \param[in] chipSelectPin SD card chip select pin.
 */
void SdSpiAltDriver::begin(uint8_t csPin) { (void)csPin; }
//------------------------------------------------------------------------------
/** Set SPI options for access to SD/SDHC cards.
 *
 */
void SdSpiAltDriver::activate() {}
//------------------------------------------------------------------------------
void SdSpiAltDriver::deactivate() {}
//------------------------------------------------------------------------------
/** Receive a byte.
 *
 * \return The byte.
 */
uint8_t SdSpiAltDriver::receive() {
  uint8_t rx = 0xFF;
  configASSERT(XSpi_Transfer(&Spi1.Device, &rx, &rx, 1) == 0);
  return rx;
}
//------------------------------------------------------------------------------
/** Receive multiple bytes.
 *
 * \param[out] buf Buffer to receive the data.
 * \param[in] n Number of bytes to receive.
 *
 * \return Zero for no error or nonzero error code.
 */
uint8_t SdSpiAltDriver::receive(uint8_t *buf, size_t n) {
  configASSERT(n > 0);
  memset(buf, 0xFF, n);
  configASSERT(XSpi_Transfer(&Spi1.Device, buf, buf, n) == 0);
  return 0;
}
//------------------------------------------------------------------------------
/** Send a byte.
 *
 * \param[in] b Byte to send
 */
void SdSpiAltDriver::send(uint8_t b) {
  configASSERT(XSpi_Transfer(&Spi1.Device, &b, NULL, 1) == 0);
}
//------------------------------------------------------------------------------
/** Send multiple bytes.
 *
 * \param[in] buf Buffer for data to be sent.
 * \param[in] n Number of bytes to send.
 */
void SdSpiAltDriver::send(uint8_t *buf, size_t n) {
  configASSERT(XSpi_Transfer(&Spi1.Device, buf, NULL, n) == 0);
}

/** Set CS low. */
void SdSpiAltDriver::select() {
  configASSERT(XSpi_SetSlaveSelect(&Spi1.Device, 1) == 0);
}
/** Set CS high. */
void SdSpiAltDriver::unselect() {
  configASSERT(XSpi_SetSlaveSelect(&Spi1.Device, 0) == 0);
}

extern "C" {
  void sd_select(void);
  void sd_unselect(void);
}

SdSpiDriver s;

void sd_select(void) {
  s.select();
}

void sd_unselect(void) {
  s.unselect();
}