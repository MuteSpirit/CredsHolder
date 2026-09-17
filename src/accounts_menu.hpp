#pragma once
#ifndef __ACCOUNTS_MENU_HPP__
#define __ACCOUNTS_MENU_HPP__

#include "menu.hpp"
#include "model.hpp"
#include "keyboard.hpp"

class Settings;
class DeviceInputs;
class Oled;

template<typename T>
class ModelStorage;


class AccountsMenu : public Menu
{
public:
    AccountsMenu(Keyboard& keyboard, Oled& oled, DeviceInputs& userInputs, const Settings&, ModelStorage<Account>&);
    ~AccountsMenu() = default;

    virtual void init(BlindCall switchMenuCb) override;

    virtual void activate() override;
    virtual void deactivate() override;

protected:
    void sendUsername();
    void sendPassword();
    void sendTab();
    void navigateAccounts(int direction);

    void draw();

    Keyboard& keyboard_;

    Oled& oled_;
    DeviceInputs& userInputs_;

    const Settings& settings_;
    ModelStorage<Account>& modelStore_;

    Account acc_;
    uint16_t acc_idx_{0};
};

#endif // !__ACCOUNTS_MENU_HPP__

