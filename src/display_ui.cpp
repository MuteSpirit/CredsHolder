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
#include "blind_call.hpp"
#include "device_inputs.hpp"
#include "accounts_menu.hpp"
#include "account_menu.hpp"
#include "settings_menu.hpp"
#include "oled.hpp"
#include "snake_game.hpp"
// #include "auth_form.hpp"


DisplayUI::DisplayUI(Oled &oled,
                     DeviceInputs &userInputs,
                     SnakeGame& tutorial,
                     // AuthForm &authForm,
                     AccountsMenu &accountsMenu,
                     AccountMenu &accMenu,
                     SettingsMenu &settingsMenu)
    : oled_(oled)
    , userInputs_(userInputs)
    , tutorial_(tutorial)
    // , authForm_(authForm)
    , accountsMenu_(accountsMenu)
    , accMenu_(accMenu)
    , settingsMenu_(settingsMenu)
{}

void
DisplayUI::setup(void)
{
    tutorial_.setup();

    BlindCall showTutorialCb(BlindCall::make(this, &DisplayUI::switch2tutorial));
    BlindCall showAccountsCb(BlindCall::make(this, &DisplayUI::switch2accountsMenu));
    BlindCall showAccountCb(BlindCall::make(this, &DisplayUI::switch2accountMenu));
    BlindCall doNothingCb(BlindCall::stub());

    // authForm_.init( ... )
    // settingsMenu_.init(...);

    //                 next            prev
    tutorial_.init(    showAccountsCb, doNothingCb);
    accountsMenu_.init(showAccountCb,  showTutorialCb);
    accMenu_.init(     doNothingCb,    showAccountsCb);

    switch2tutorial();
}

void
DisplayUI::loop_step(void)
{
    if (State::tutorial == state_) {
        tutorial_.loop_step();
    }
}

void
DisplayUI::switch2tutorial()
{
    accountsMenu_.deactivate();

    tutorial_.activate();
    tutorial_.draw();

    state_ = State::tutorial;
}

// void
// DisplayUI::switch2settingsMenu()
// {
//     // accMenu_.deactivate();
//     settingsMenu_.activate();
//
//     userInputs_.set(DeviceInputs::UserAction::up, BlindCall::make(this, &DisplayUI::switch2accountsMenu));
// }

void
DisplayUI::switch2accountsMenu()
{
    tutorial_.deactivate();
    accMenu_.deactivate();

    accountsMenu_.activate();
    accountsMenu_.draw();

    state_ = State::accounts;
}

void
DisplayUI::switch2accountMenu()
{
    accountsMenu_.deactivate();

    accMenu_.account(accountsMenu_.selected());
    accMenu_.activate();
    accMenu_.draw();

    state_ = State::account;
}
