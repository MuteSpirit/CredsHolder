// Copyright (C)  2026  Ivan Efimov aka MuteSpirit <mutespirit@yandex.ru>.
//
// Permission is granted to copy, distribute and/or modify this document
// under the terms of the GNU Free Documentation License, Version 1.3
// or any later version published by the Free Software Foundation;
// with no Invariant Sections, no Front-Cover Texts, and no Back-Cover Texts.
// A copy of the license is included in the section entitled "GNU
// Free Documentation License".
#pragma once

#include "device_inputs.hpp"

#if defined(ARDUINO_ARCH_AVR) // Arduino

// OLED Display
#define OLED_SDA_PIN    2
#define OLED_SCK_PIN    3
#define OLED_I2C_ADDR   0x3C

#elif defined(ARDUINO_ARCH_NRF52) // NRF52840

// OLED Display
#define OLED_SDA_PIN    D6 
#define OLED_SCK_PIN    D7
#define OLED_I2C_ADDR   0x3C

#define MPU6050_CS_PIN  D5

#else
#error("Unknown board type")
#endif // defined(ARDUINO_ARCH_AVR)

class CredsHolderInputsData;

class CredsHolderInputs : public DeviceInputs
{
public:
    CredsHolderInputs();

    virtual void set(UserAction act, BlindCall cb) override;
    virtual void unset(UserAction act) override;

    /// @return "true" if Ok and "false" if initializing fail and device cannot be used, init reboot in that case.
    virtual bool setup(void) override;

    virtual void loop_step(void) override;

protected:
    CredsHolderInputsData* data();

protected:
    uint8_t data_[1024]; /// "insulation' for internal data
};
