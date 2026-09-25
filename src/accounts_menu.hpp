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
