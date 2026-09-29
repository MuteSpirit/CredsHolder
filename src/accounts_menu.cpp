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
#include <new>
#include <Arduino.h>
#include <etl/iterator.h>

#include "blind_call.hpp"
#include "device.hpp"
#include "model/storage.hpp"
#include "settings.hpp"
#include "oled.hpp"
#include "ui_selection_list.hpp"


////////////////////////////////////////////////////////////////////////////////
class AccountsMenuImpl
{
public:
    AccountsMenuImpl(Oled& oled, DeviceInputs& userInputs, const Settings&, ModelStorage<Account>&);

    void prev();
    void next();
    // void navigateAccounts(int direction);

    void select();

public:
    BlindCall nextMenuCb_; /// jump to form showing concrete Account
    BlindCall prevMenuCb_; /// jump back to form showed before "Accounts'

    Oled& oled_;
    DeviceInputs& userInputs_;

    const Settings& settings_;
    ModelStorage<Account>& modelStore_;

    etl::uniquie_ptr<UISelectionList<AccountIterator, Account>> accSl_;
};

////////////////////////////////////////////////////////////////////////////////
AccountsMenu::AccountsMenu(Oled& oled,
                           DeviceInputs& userInputs,
                           const Settings& settings,
                           ModelStorage<Account>& modelStore)
{
    static_assert(sizeof(impl_) == sizeof(AccountsMenuImpl), "fix AccountsMenu::impl_ size");
    new (impl_) AccountsMenuImpl(oled, userInputs, settings, modelStore);
}

AccountsMenuImpl*
AccountsMenu::impl()
{
    return reinterpret_cast<AccountsMenuImpl*>(impl_);
}

const AccountsMenuImpl*
AccountsMenu::impl() const
{
    return reinterpret_cast<const AccountsMenuImpl*>(impl_);
}

void
AccountsMenu::init(BlindCall nextMenuCb, BlindCall prevMenuCb)
{
    impl()->nextMenuCb_ = nextMenuCb;
    impl()->prevMenuCb_ = prevMenuCb;
}

void
AccountsMenu::activate()
{
    if (Serial) {Serial.println(F("AccountsMenu::activate"));}

    impl()->userInputs_.set(DeviceInputs::UserAction::left, impl()->prevMenuCb_);
    impl()->userInputs_.set(DeviceInputs::UserAction::right, BlindCall::make(impl(), &AccountsMenuImpl::select));

    impl()->userInputs_.set(DeviceInputs::UserAction::up, BlindCall::make(impl(), &AccountsMenuImpl::prev));
    impl()->userInputs_.set(DeviceInputs::UserAction::down, BlindCall::make(impl(), &AccountsMenuImpl::next));

    impl()->oled_.home();

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
    impl()->userInputs_.unset(DeviceInputs::UserAction::left);
    impl()->userInputs_.unset(DeviceInputs::UserAction::down);
    impl()->userInputs_.unset(DeviceInputs::UserAction::right);
    impl()->userInputs_.unset(DeviceInputs::UserAction::enter);

    // acc_idx_ = 0;
    // memset(&acc_, 0, sizeof(Account));

    impl()->oled_.clear();
}

////////////////////////////////////////////////////////////////////////////////
AccountsMenuImpl::AccountsMenuImpl(Oled& oled,
                                   DeviceInputs& userInputs,
                                   const Settings& settings,
                                   ModelStorage<Account>& modelStore)
    : oled_(oled)
    , userInputs_(userInputs)
    , settings_(settings)
    , modelStore_(modelStore)
    , accSl_(new UISelectionList<AccountIterator, Account>(oled_, "Credentials", cbegin(modelStore_), cend(modelStore_)))
{
}

void
AccountsMenuImpl::select()
{
    if (selectItemCb_) {
        selectItemCb_(accSl_->selected());
    }
}

void
AccountsMenuImpl::next()
{
    if (Serial) { Serial.println(F("AccountsMenu::next")); }

    accSl_->next();
}

void
AccountsMenuImpl::prev()
{
    if (Serial) { Serial.println(F("AccountsMenu::prev")); }

    accSl_->prev();
}

// /// @param[in] direction - 0 = No rotation, 1 = Clockwise, -1 = Counter Clockwise
// void
// AccountsMenu::navigateAccounts(int direction)
// {
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
// }

void
AccountsMenu::draw()
{
    accSl_->draw();

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



