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

#if defined(ARDUINO_ARCH_AVR)
#include "../keyboard.hpp"
#include <Keyboard.h> // for simulating a USB keyboard and sending output to it

bool
ArduinoKeyboard::print(const char* s)
{
// for Uno it cannot be even compiled because that board cannot emulate keyboard
#if defined(_USING_HID)
    Keyboard.begin();
    Keyboard.print(s);
    Keyboard.end();
#endif
    return true;
}

bool
ArduinoKeyboard::push(const uint8_t key_code)
{
// for Uno it cannot be even compiled because that board cannot emulate keyboard
#if defined(_USING_HID)
    Keyboard.begin(); // TODO: can we do a <CTL><A> <BS> here first?  That will clear out pre-populated usernames.
    Keyboard.press(key_code);
    Keyboard.release(key_code);
    Keyboard.end();
#endif
    return true;
}

bool
ArduinoKeyboard::push_tab()
{
#if defined(_USING_HID)
    push(KEY_TAB);
#endif
    return true;
}
#endif // defined(ARDUINO_ARCH_AVR)
