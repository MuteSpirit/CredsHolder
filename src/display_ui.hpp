#pragma once
#ifndef __DISPLAY_UI_HPP__
#define __DISPLAY_UI_HPP__

class Oled;
class AccountsMenu;
class SettingsMenu;
class AuthForm;
class DeviceInputs;


class DisplayUI
{
public:
    DisplayUI(Oled&, DeviceInputs &, AuthForm &, AccountsMenu&, SettingsMenu&);

    void ui_setup(void);

protected:
    void switch2settingsMenu();
    void switch2accountsMenu();

protected:
    Oled& oled_;
    DeviceInputs &userInputs_;
    AuthForm &authForm_;
    AccountsMenu& accMenu_;
    SettingsMenu& settingsMenu_;
};


#endif // !__DISPLAY_UI_HPP__
