// Copyright (C)  2026  Ivan Efimov aka MuteSpirit <mutespirit@yandex.ru>.
//
// Permission is granted to copy, distribute and/or modify this document
// under the terms of the GNU Free Documentation License, Version 1.3
// or any later version published by the Free Software Foundation;
// with no Invariant Sections, no Front-Cover Texts, and no Back-Cover Texts.
// A copy of the license is included in the section entitled "GNU
// Free Documentation License".
#pragma once

#include "oled.hpp"
#include <cstddef>

#include <U8g2lib.h>
// Rejected OLED displays libraries
// * SSD1306AsciiAvrI2c - for Arduino AVR only and not supported on NRF52840
// * GyverOLED does not allow to set font
// * GyverOLEDMenu draw ugly menu items


class SSD1306I2C : public Oled
{
public:
   virtual void setup() override; 

   virtual void clear() override; 
   virtual void home() override; 

   void setFont(const uint8_t* font) override;

   virtual size_t write(uint8_t) override;
   virtual size_t write(const uint8_t *buffer, size_t size) override;

protected:
   U8G2 u8g2_;
};
