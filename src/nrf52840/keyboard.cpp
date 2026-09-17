// Copyright (C)  2026  Ivan Efimov aka MuteSpirit <mutespirit@yandex.ru>.
//
// Permission is granted to copy, distribute and/or modify this document
// under the terms of the GNU Free Documentation License, Version 1.3
// or any later version published by the Free Software Foundation;
// with no Invariant Sections, no Front-Cover Texts, and no Back-Cover Texts.
// A copy of the license is included in the section entitled "GNU
// Free Documentation License".

#include "keyboard.hpp"

static uint8_t const desc_keyboard_report[] = {
    TUD_HID_REPORT_DESC_KEYBOARD()
};

void TinyUsbKeyboard::setup()
{
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
    if (!ready()) {
        return false;
    }

    for (const char *p = s; p; ++p) {
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

    return true;
}

bool TinyUsbKeyboard::push_tab()
{
    return push(HID_KEY_TAB);
}
