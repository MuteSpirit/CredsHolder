// Copyright (C)  2026  Ivan Efimov aka MuteSpirit <mutespirit@yandex.ru>.
//
// Permission is granted to copy, distribute and/or modify this document
// under the terms of the GNU Free Documentation License, Version 1.3
// or any later version published by the Free Software Foundation;
// with no Invariant Sections, no Front-Cover Texts, and no Back-Cover Texts.
// A copy of the license is included in the section entitled "GNU
// Free Documentation License".
#pragma once

#include <inttypes.h>
#include <cstddef>

#if defined(ARDUINO_ARCH_NRF52)
#define NRF52840
#  if !defined(USE_TINYUSB)
#  define USE_TINYUSB 1
#  endif
#endif
#include <Print.h>

class Oled
#if defined(ARDUINO)
: public Print
#endif
{
public:
    virtual void setup() = 0; 

    virtual void clear() = 0; 
    virtual void home() = 0; 

    virtual void setFont(const uint8_t* font) = 0;

    virtual ~Oled() {};

    virtual size_t write(uint8_t) = 0;
    virtual size_t write(const uint8_t *buffer, size_t size) = 0;

protected:
    Oled() = default;
};
