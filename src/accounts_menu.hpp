// Copyright (C)  2026  Ivan Efimov aka MuteSpirit <mutespirit@yandex.ru>.
//
// Permission is granted to copy, distribute and/or modify this document
// under the terms of the GNU Free Documentation License, Version 1.3
// or any later version published by the Free Software Foundation;
// with no Invariant Sections, no Front-Cover Texts, and no Back-Cover Texts.
// A copy of the license is included in the section entitled "GNU
#pragma once
#include "creds_holder.hpp"

#include "menu.hpp"
#include "model.hpp"
#include "keyboard.hpp"
#include "blind_call.hpp"
#include "ui_selection_list.hpp"

class Settings;
class DeviceInputs;
class Oled;

template<typename T>
class ModelStorage;


class AccountsMenu : public Menu
{
public:
    AccountsMenu(Oled& oled, DeviceInputs& userInputs, const Settings&, ModelStorage<Account>&);
    ~AccountsMenu() = default;

    virtual void init(BlindCall nextMenuCb, BlindCall prevMenuCb) override;

    virtual void activate() override;
    virtual void deactivate() override;

    virtual void draw() override;

    // Account selected() const;

protected:
    void prevAcc();
    void nextAcc();
    void navigateAccounts(int direction);

    void selectAcc();

    BlindCall selectItemCb_; /// jump to form showing concrete Account
    BlindCall returnCb_; /// jump back to form showed before "Accounts'

protected:
    Oled& oled_;
    DeviceInputs& userInputs_;

    const Settings& settings_;
    ModelStorage<Account>& modelStore_;

    // UISelectionList *accSl_;
};
