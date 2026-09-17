// Copyright (C)  2026  Ivan Efimov aka MuteSpirit <mutespirit@yandex.ru>.
//
// Permission is granted to copy, distribute and/or modify this document
// under the terms of the GNU Free Documentation License, Version 1.3
// or any later version published by the Free Software Foundation;
// with no Invariant Sections, no Front-Cover Texts, and no Back-Cover Texts.
// A copy of the license is included in the section entitled "GNU
// Free Documentation License".
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
