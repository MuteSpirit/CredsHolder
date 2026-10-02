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

#include "menu.hpp"

class Oled;
class AccountMenuImpl;
class DeviceInputs;
class Keyboard;
struct Account;

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
    /// @details Use static_assert in constructor to check required size at compile time 
    uint8_t impl_[200];
};
