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
#include "accounts_menu.hpp"

#include "creds_holder.hpp"
#include <Arduino.h>

#include "blind_call.hpp"
#include "device.hpp"
#include "keyboard.hpp"
#include "model_storage.hpp"
#include "settings.hpp"
#include "oled.hpp"


AccountsMenu::AccountsMenu(Oled& oled,
                           DeviceInputs& userInputs,
                           const Settings& settings,
                           ModelStorage<Account>& modelStore)
    : oled_(oled)
    , userInputs_(userInputs)
    , settings_(settings)
    , modelStore_(modelStore)
    // , accSl_(new_sl(oled))
{
}

void
AccountsMenu::selectAcc()
{
    if (selectItemCb_) {
        selectItemCb_();
    }
}

void
AccountsMenu::init(BlindCall nextMenuCb, BlindCall prevMenuCb)
{
    selectItemCb_ = nextMenuCb;
    returnCb_ = prevMenuCb;
}

void
AccountsMenu::activate()
{
    if (Serial) {Serial.println(F("AccountsMenu::activate"));}

    // userInputs_.set(DeviceInputs::UserAction::left, BlindCall::make(this, &AccountsMenu::sendUsername));
    // userInputs_.set(DeviceInputs::UserAction::right, BlindCall::make(this, &AccountsMenu::sendTab));

    userInputs_.set(DeviceInputs::UserAction::up, BlindCall::make(this,&AccountsMenu::prevAcc));
    userInputs_.set(DeviceInputs::UserAction::down, BlindCall::make(this, &AccountsMenu::nextAcc));

    oled_.home();

    // if (modelStore_.get(acc_idx_, acc_) || 
    //     modelStore_.getNext(acc_idx_, acc_, acc_idx_)) {
    //     draw();
    // } else {
    //     oled_.println(F("No creds accounts"));
    // }
}

void
AccountsMenu::deactivate()
{
    userInputs_.unset(DeviceInputs::UserAction::left);
    userInputs_.unset(DeviceInputs::UserAction::down);
    userInputs_.unset(DeviceInputs::UserAction::right);
    userInputs_.unset(DeviceInputs::UserAction::enter);

    // acc_idx_ = 0;
    // memset(&acc_, 0, sizeof(Account));

    oled_.clear();
}

void
AccountsMenu::nextAcc()
{
    if (Serial) { Serial.println(F("AccountsMenu::nextAcc")); }

    navigateAccounts(1);
}

void
AccountsMenu::prevAcc()
{
    if (Serial) { Serial.println(F("AccountsMenu::prevAcc")); }

    navigateAccounts(-1);
}

/// @param[in] direction - 0 = No rotation, 1 = Clockwise, -1 = Counter Clockwise
void
AccountsMenu::navigateAccounts(int direction)
{
    (void)direction;
    // if (0 == direction) {
    //     return;
    // }
    //
    // ModelStorage<Account>::ObjIndex idx = acc_idx_;
    //
    // do {
    //     if (direction > 0) {
    //         if (modelStore_.getNext(acc_idx_, acc_, idx)) {
    //             break;
    //         }
    //         if (modelStore_.get(0, acc_)) {
    //             idx = 0;
    //             break;
    //         }
    //         if (modelStore_.getNext(0, acc_, idx)) {
    //             break;
    //         }
    //     } else {
    //         if (modelStore_.getPrev(acc_idx_, acc_, idx)) {
    //             break;
    //         }
    //         auto maxIdx = modelStore_.maxIdx();
    //
    //         if (modelStore_.get(maxIdx, acc_)) {
    //             idx = maxIdx;
    //             break;
    //         }
    //         if (modelStore_.getPrev(maxIdx, acc_, idx)) {
    //             break;
    //         }
    //     }
    // } while (0);
    //
    // if (idx != acc_idx_) {
    //     acc_idx_ = idx;
    //
    //     oled_.clear();
    //     oled_.home();
    //
    //     draw();
    // }
}

void
AccountsMenu::draw()
{
    // if (Serial) {Serial.println(F("AccountsMenu::draw"));}
    //
    // oled_.println("  Credentials:  ");
    //
    // oled_.print(F("N: "));
    // oled_.println(acc_.name);
    //
    // oled_.print(F("U: "));
    // oled_.println(acc_.username);
    //
    // oled_.print(F("P: "));
    // if (settings_.unhide_passwords_) {
    //     oled_.println(acc_.password);
    // } else {
    //     char hidden_passwd[PASSWORD_SIZE] = {0};
    //     const uint8_t len = strnlen(acc_.password, PASSWORD_SIZE);
    //     for (uint8_t i = 0; i < len; ++i) {
    //         hidden_passwd[i] = '*';
    //     }
    //     oled_.println(hidden_passwd);
    // }
}

// Account
// AccountsMenu::selected() const
// {
//     // modelStore_.search(selected(), Account &t)
// }
