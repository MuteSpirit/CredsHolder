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
