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
