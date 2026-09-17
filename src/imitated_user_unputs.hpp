// Copyright (C)  2026  Ivan Efimov aka MuteSpirit <mutespirit@yandex.ru>.
//
// Permission is granted to copy, distribute and/or modify this document
// under the terms of the GNU Free Documentation License, Version 1.3
// or any later version published by the Free Software Foundation;
// with no Invariant Sections, no Front-Cover Texts, and no Back-Cover Texts.
// A copy of the license is included in the section entitled "GNU
#pragma once

#include "device_inputs.hpp"

class ImitatedUserInputs : public DeviceInputs
{
public:
    ImitatedUserInputs() = default;
    ~ImitatedUserInputs() = default;
    
    virtual void set(UserAction act, BlindCall cb) override;
    virtual void unset(UserAction act) override;

    virtual bool setup(void) override { return true; };
    virtual void loop_step(void) override {};

    void click(UserAction act);

protected:
    BlindCall hooks_[static_cast<uint8_t>(DeviceInputs::UserAction::size)];
};
