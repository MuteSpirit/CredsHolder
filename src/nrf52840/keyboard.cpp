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

#if defined(ARDUINO_ARCH_NRF52)
#include "keyboard.hpp"

#include <Embedded_Template_Library.h>
#include <etl/array.h>

static uint8_t const desc_keyboard_report[] = {
    TUD_HID_REPORT_DESC_KEYBOARD()
};

void TinyUsbKeyboard::setup()
{
    // if (Serial) { Serial.println(F("TinyUsbKeyboard::setup()")); }

    if (!TinyUSBDevice.isInitialized()) {
        TinyUSBDevice.begin(0);
    }

    // HID Keyboard
    usb_keyboard_.setPollInterval(10 /*ms*/); // default is 4 ms
    usb_keyboard_.setBootProtocol(HID_ITF_PROTOCOL_KEYBOARD);
    usb_keyboard_.setReportDescriptor(desc_keyboard_report, sizeof(desc_keyboard_report));
    usb_keyboard_.setStringDescriptor("CredsHolder HW Credential Manager");

    usb_keyboard_.begin();

    // If already enumerated, additional class driverr begin() e.g msc, hid, midi won't take effect until re-enumeration
    if (TinyUSBDevice.mounted()) {
        TinyUSBDevice.detach();
        delay(2000);
        TinyUSBDevice.attach();
    }
}

bool TinyUsbKeyboard::ready()
{
    return TinyUSBDevice.mounted() && usb_keyboard_.ready();
}

bool TinyUsbKeyboard::print(const char* s)
{
    if (TinyUSBDevice.suspended()) {
        // Wake up host if we are in suspend mode
        // and REMOTE_WAKEUP feature is enabled by host
        TinyUSBDevice.remoteWakeup();
    }

    for (auto i = 1; i <= 10; ++i) {
        if (!ready()) {
            delay(i * 50);
        }
    }

    if (!ready()) {
        if (Serial) { Serial.println(F("Keyboard is not ready")); }
        return false;
    }

    for (const char *p = s; *p; ++p) {
        usb_keyboard_.keyboardPress(0, *p);
        delay(10);
        usb_keyboard_.keyboardRelease(0);
        delay(10);
    }

    return true;
}

bool TinyUsbKeyboard::push(const uint8_t key_code)
{
    if (!ready()) {
        return false;
    }

    uint8_t keycode[6] = {0};
    keycode[0] = key_code;

    usb_keyboard_.keyboardReport(0, 0, keycode);
    delay(10);
    usb_keyboard_.keyboardRelease(0);
    delay(10);

    return true;
}

bool TinyUsbKeyboard::push_tab()
{
    return push(HID_KEY_TAB);
}

#endif // defined(ARDUINO_ARCH_NRF52)
