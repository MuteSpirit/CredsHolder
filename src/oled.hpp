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

#include <inttypes.h>

#include <Print.h>

// Include header with fonts declarations to be able use setFont method
// FIXME: Choose only needed fonts, make separate module with them (or 
// find a way to cut the others). The goal - avoid concrete UI library header 
// direct usage
#include <clib/u8x8.h>

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
