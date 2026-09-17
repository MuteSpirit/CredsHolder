// Copyright (C)  2026  Ivan Efimov aka MuteSpirit <mutespirit@yandex.ru>.
//
// Permission is granted to copy, distribute and/or modify this document
// under the terms of the GNU Free Documentation License, Version 1.3
// or any later version published by the Free Software Foundation;
// with no Invariant Sections, no Front-Cover Texts, and no Back-Cover Texts.
// A copy of the license is included in the section entitled "GNU
// Free Documentation License".

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
