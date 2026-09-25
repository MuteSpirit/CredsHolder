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

class Oled;
class AccountsMenu;
// class AccountMenu;
class SettingsMenu;
class AuthForm;
class DeviceInputs;


class DisplayUI
{
public:
    DisplayUI(Oled&, DeviceInputs &,/* AuthForm &, */AccountsMenu&/*, AccountMenu&*/, SettingsMenu&);

    void setup(void);

protected:
    void switch2accountsMenu(); /// acc -> accounts
    void switch2accountMenu();  /// accounts -> acc
    void switch2settingsMenu();

protected:
    Oled& oled_;
    DeviceInputs &userInputs_;
    // AuthForm &authForm_;
    AccountsMenu& accountsMenu_;
    // AccountMenu& accMenu_;
    SettingsMenu& settingsMenu_;
};
