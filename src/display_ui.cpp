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
#include "display_ui.hpp"
#include "device_inputs.hpp"
#include "accounts_menu.hpp"
#include "account_menu.hpp"
#include "settings_menu.hpp"
#include "oled.hpp"
// #include "auth_form.hpp"


DisplayUI::DisplayUI(Oled &oled,
                     DeviceInputs &userInputs,
                     // AuthForm &authForm,
                     AccountsMenu &accountsMenu,
                     // AccountMenu &accMenu,
                     SettingsMenu &settingsMenu)
    : oled_(oled)
    , userInputs_(userInputs)
    // , authForm_(authForm)
    , accountsMenu_(accountsMenu)
    // , accMenu_(accMenu)
    , settingsMenu_(settingsMenu)
{}

void
DisplayUI::setup(void)
{
    oled_.clear();
    oled_.home();

    // authForm_.init(BlindCall::make(this, &DisplayUI::switch2accountsMenu), BlindCall::stub());
    accountsMenu_.init(BlindCall::make(this, &DisplayUI::switch2accountMenu), BlindCall::make(this, &DisplayUI::switch2settingsMenu));
    // accMenu_.init(BlindCall::make(this, &DisplayUI::switch2accountMenu), BlindCall::make(this, &DisplayUI::switch2accountsMenu));
    settingsMenu_.init(BlindCall::make(this, &DisplayUI::switch2accountsMenu), BlindCall::make(this, &DisplayUI::switch2settingsMenu));
    //
    switch2accountsMenu();
}

void
DisplayUI::switch2settingsMenu()
{
    // accMenu_.deactivate();
    settingsMenu_.activate();

    userInputs_.set(DeviceInputs::UserAction::up, BlindCall::make(this, &DisplayUI::switch2accountsMenu));
}

void
DisplayUI::switch2accountsMenu()
{
    // accMenu_.deactivate();
    accountsMenu_.activate();
}

void
DisplayUI::switch2accountMenu()
{
    accountsMenu_.deactivate();

    // accMenu_.init(accountsMenu_.selected());
    // accMenu_.activate();
}
