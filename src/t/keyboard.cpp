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
#if defined(EPOXY_DUINO)
#include "keyboard.hpp"

bool
ImitatedKeyboard::print(const char* s)
{
    istream_ += s;
    return true;
}

bool
ImitatedKeyboard::push(const uint8_t keyCode)
{
    (void)keyCode;
    // FIXME: need to take/include "class/hid/hid.h" from Adafruit_TinyUSB_Library 
    // because AUnit does not support it and include does not work

    // uint8_t const conv_table[128][2] =  { HID_KEYCODE_TO_ASCII };
    // bool shift = false;
    // char ch = shift ? conv_table[keyCode][1] : conv_table[keyCode][0];
    // istream_ += ch;
    return true;
}

bool
ImitatedKeyboard::push_tab()
{
    istream_ += '\t';
    return true;
}

const char*
ImitatedKeyboard::istream() const
{
    return istream_.c_str();
}

#endif // defined(EPOXY_DUINO)
