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

#include "blind_call.hpp"

/// Interface to set callbacks for device abstract inputs - forward, backward, up, down, etc.
/// The same type of device may have different input modules but perform the same actions, e.g.
///    encoder + 4 buttons may be replaced with ...
///      ... accelerometer, gyroscope and piezo sensor
///      ... or touch screen
/// UI forms should be abstracted on that to avoid code fragileness.
/// Class should be used for writing UI menu/forms unit test.
class DeviceInputs
{
public:
    enum class UserAction : uint8_t
    {
        up,
        down,
        left,
        right,
        enter,
        size
    };

    /// "cb" will be called on happen user action "act"
    virtual void set(UserAction act, BlindCall cb) = 0;

    /// Disable reaction on user action "act"
    virtual void unset(UserAction act) = 0;

    virtual bool setup(void) = 0; /// will be called in "setup" sketch function
    virtual void loop_step(void) = 0; /// will be called in "loop" sketch function

    virtual ~DeviceInputs() = default;
    
protected:
    DeviceInputs() = default;
};
