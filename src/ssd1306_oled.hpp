// Copyright (C)  2026  Ivan Efimov aka MuteSpirit <mutespirit@yandex.ru>.
//
// Permission is granted to copy, distribute and/or modify this document
// under the terms of the GNU Free Documentation License, Version 1.3
// or any later version published by the Free Software Foundation;
// with no Invariant Sections, no Front-Cover Texts, and no Back-Cover Texts.
// A copy of the license is included in the section entitled "GNU
// Free Documentation License".
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
