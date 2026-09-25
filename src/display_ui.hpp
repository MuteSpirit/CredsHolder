#pragma once
#ifndef __DISPLAY_UI_HPP__
#define __DISPLAY_UI_HPP__

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


#endif // !__DISPLAY_UI_HPP__
