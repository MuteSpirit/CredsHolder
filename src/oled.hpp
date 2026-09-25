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

#include <inttypes.h>
// #include "etl/memory.h"

#include <Print.h>

// TODO: Rename class to Gui
/// @details Coordinates increases from left top corner (0, 0), for example:
/// +----------------------------------------------+
/// | (0, 0) | (0, 1) | ...                        |
/// | (1, 0) | (1, 1) | ...                        |
/// | ...                                          |
/// | ...             | (getCols()-1, getRows()-1) |
/// +----------------------------------------------+
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
    virtual void setInverseFont(uint8_t value) = 0;
    virtual void drawUTF8(uint8_t col, uint8_t row, const char *s) = 0;

    virtual ~Oled() {};

    /// Update OLED with updated info from buffer
    virtual void display() = 0;

    virtual uint8_t getRows(void) = 0;
    virtual uint8_t getCols(void) = 0;

protected:
    Oled() = default;
};
