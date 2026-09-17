// Copyright (C)  2026  Ivan Efimov aka MuteSpirit <mutespirit@yandex.ru>.
//
// Permission is granted to copy, distribute and/or modify this document
// under the terms of the GNU Free Documentation License, Version 1.3
// or any later version published by the Free Software Foundation;
// with no Invariant Sections, no Front-Cover Texts, and no Back-Cover Texts.
// A copy of the license is included in the section entitled "GNU
// Free Documentation License".
#pragma once

#if defined(ARDUINO_ARCH_NRF52)
#include "../keyboard.hpp"

#include "Adafruit_TinyUSB.h"

class TinyUsbKeyboard : public Keyboard
{
public:
    void setup();
    void loop_step();

    virtual bool print(const char* s) override;
    virtual bool push(const uint8_t key_code) override;
    virtual bool push_tab() override;

protected:
    bool ready();

protected:
    Adafruit_USBD_HID usb_keyboard_;
};
#endif // defined(ARDUINO_ARCH_NRF52)
