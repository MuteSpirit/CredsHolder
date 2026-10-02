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

#include <inttypes.h>

/// @brief Class allow menus signal user about some event.
///
/// @details Abstraction above vibration motor module, LED, and other 
/// modules allowing give feedback to user.
/// Currently planned events:
/// 1. Device tilt angle is enough to recognize it as User action
/// 2. Inform User about currently chosen digit by Morse code
class DeviceOutputs
{
public:
    enum class Feedback : uint8_t
    {
        NONE,
        halfTilt, // device is tilted but not yet returned to horizontal position
        END
    };

public:
    virtual void notify(Feedback) = 0;

    virtual bool setup(void) = 0; /// will be called in "setup" sketch function
    virtual void loop_step(void) = 0; /// will be called in "loop" sketch function

protected:
    virtual ~DeviceOutputs() = default;
};
