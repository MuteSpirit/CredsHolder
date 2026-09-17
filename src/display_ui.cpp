#include "display_ui.hpp"
#include "device_inputs.hpp"
#include "accounts_menu.hpp"
#include "settings_menu.hpp"
#include "oled.hpp"
#include "auth_form.hpp"


DisplayUI::DisplayUI(Oled &oled,
                     DeviceInputs &userInputs,
                     AuthForm &authForm,
                     AccountsMenu &accMenu,
                     SettingsMenu &settingsMenu)
    : oled_(oled)
    , userInputs_(userInputs)
    , authForm_(authForm)
    , accMenu_(accMenu)
    , settingsMenu_(settingsMenu)
{}

void
DisplayUI::ui_setup(void)
{
    oled_.clear();
    oled_.home();

    authForm_.init(BlindCall::make(this, &DisplayUI::switch2accountsMenu), BlindCall::stub());
    accMenu_.init(BlindCall::make(this, &DisplayUI::switch2settingsMenu));
    settingsMenu_.init(BlindCall::make(this, &DisplayUI::switch2accountsMenu));

    switch2accountsMenu();
}

void
DisplayUI::switch2settingsMenu()
{
    accMenu_.deactivate();
    settingsMenu_.activate();

    userInputs_.set(DeviceInputs::UserAction::up, BlindCall::make(this, &DisplayUI::switch2accountsMenu));
}

void
DisplayUI::switch2accountsMenu()
{
    settingsMenu_.deactivate();
    accMenu_.activate();

    userInputs_.set(DeviceInputs::UserAction::up, BlindCall::make(this, &DisplayUI::switch2settingsMenu));
}
