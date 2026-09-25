// CredsHolder (Hardware Credential Manager)
// Copyright (C)  2026  Ivan Efimov aka MuteSpirit <mutespirit@yandex.ru>.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
#pragma once

#include "creds_holder.hpp"

#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

#include <U8x8lib.h>
// Rejected OLED displays libraries
// * SSD1306AsciiAvrI2c - for Arduino AVR only and not supported on NRF52840
// * GyverOLED does not allow to set font
// * GyverOLEDMenu draw ugly menu items

#include "ui_selection_list.hpp"
#include "oled.hpp"

/// SSD1306 I2C OLED 128x64 display class
class SSD1306I2C : public Oled
{
public:
   virtual void setup() override; 

   virtual void clear() override; 
   virtual void home() override; 

   void setFont(const uint8_t* font) override;
   virtual void setInverseFont(uint8_t value) override;

   virtual size_t write(uint8_t) override;
   virtual size_t write(const uint8_t *buffer, size_t size) override;
   virtual void drawUTF8(uint8_t col, uint8_t row, const char *s) override;

   virtual void display() override;

   virtual uint8_t getRows(void) override;
   virtual uint8_t getCols(void) override;

protected:
   U8X8_SSD1306_128X64_NONAME_HW_I2C u8x8_ {/* reset=*/ U8X8_PIN_NONE};

   // friend etl::unique_ptr<UISelectionList> new_sl(const char* title,
   //                                               Oled& oled, 
   //                                               typename UISelectionList::const_iterator start, 
   //                                               typename UISelectionList::const_iterator end);
};
