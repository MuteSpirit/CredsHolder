// Copyright (C)  2026  Ivan Efimov aka MuteSpirit <mutespirit@yandex.ru>.
//
// Permission is granted to copy, distribute and/or modify this document
// under the terms of the GNU Free Documentation License, Version 1.3
// or any later version published by the Free Software Foundation;
// with no Invariant Sections, no Front-Cover Texts, and no Back-Cover Texts.
// A copy of the license is included in the section entitled "GNU
// Free Documentation License".
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
