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

#include <inttypes.h>
#include "blind_call.hpp"

// Interface to set callbacks for device inputs (buttons, rotate encoder)
// Should be used to write unit test for UI menu/forms
class UserInputs
{
public:
    /// There are 4 buttons on the device with geometric figures on them:
    // TODO: support button in rotary encoder too
    enum class Button : uint8_t
    {
       square = 0,
       triangle,
       circle,
       cross,
       rotary, // button of rotary encoder
       num_of_buttons
    };

    enum class Encoder : uint8_t
    {
        rotary = 0,
        num_of_encoders
    };

    virtual void set(Button btn, BlindCall cb) = 0; /// on button click (== push+release)
    virtual void unset(Button btn) = 0; /// delete callback previously set on button

    virtual void set(Encoder, BlindCall cb) = 0; /// expected call is cb(int direction)
    virtual void unset(Encoder) = 0; /// delete callback previously set on encoder

    virtual ~UserInputs() = default;
    
protected:
    UserInputs() = default;
};
