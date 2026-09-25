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
#include "creds_holder.hpp"

#include "etl/iterator.h"

#include "menu.hpp"

class Oled;
class AccountMenuImpl;
class DeviceInputs;
class Keyboard;
class Account;

/// GUI element to show single credential account, it's URL, login, password, etc.
/// Account fields which maybe typed into login form (or other) will be able to navigate and select.
/// On selection some field CredsHolder must type it on attached PC.
class AccountMenu : public Menu
{
public:
    AccountMenu(Oled &oled, DeviceInputs&, Keyboard& keyboard);

    void account(const Account& acc);
    void account(const Account&& acc);

    const Account& account() const;

    virtual void init(BlindCall nextMenuCb, BlindCall prevMenuCb) override;

    virtual void activate() override;
    virtual void deactivate() override;

    virtual void draw() override;

protected:
    AccountMenuImpl* impl();
    const AccountMenuImpl* impl() const;

protected:
    /// Buffer for implementation class instance
    /// @details Compiler will say required size via similar message:
    ///   src/account_menu.cpp|40 col 10| warning: placement new constructing an object of type 'AccountMenuImpl' and size '180' in a region of type 'uint8_t [16]' {aka 'unsigned char [16]'} and size '16' [-Wplacement-new=]
    ///   ||    40 |     new (impl_) AccountMenuImpl(keyboard, oled, userInputs);
    ///   ||       |          ^~~~~
    uint8_t impl_[200];
};
