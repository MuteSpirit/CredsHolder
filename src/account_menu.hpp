// Copyright (C)  2026  Ivan Efimov aka MuteSpirit <mutespirit@yandex.ru>.
//
// Permission is granted to copy, distribute and/or modify this document
// under the terms of the GNU Free Documentation License, Version 1.3
// or any later version published by the Free Software Foundation;
// with no Invariant Sections, no Front-Cover Texts, and no Back-Cover Texts.
// A copy of the license is included in the section entitled "GNU
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
