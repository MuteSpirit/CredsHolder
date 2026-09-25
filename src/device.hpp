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

#include "device_inputs.hpp"

#if defined(ARDUINO_ARCH_AVR) // Arduino

// OLED Display
#define OLED_SDA_PIN    2
#define OLED_SCK_PIN    3
#define OLED_I2C_ADDR   0x3C

#define MPU6050_CS_PIN  D5

#elif defined(ARDUINO_ARCH_NRF52) // Pro Micro nRF52840

// OLED Display
#define OLED_SDA_PIN    D6
#define OLED_SCK_PIN    D7
#define OLED_I2C_ADDR   0x3C

#define MPU6050_CS_PIN  D5

#elif defined(EPOXY_DUINO) // Unit tests

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
