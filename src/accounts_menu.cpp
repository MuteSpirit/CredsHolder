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

#include "Embedded_Template_Library.h"
#include "etl/memory.h"

#include "model/storage.hpp"
#include "model/iterator.hpp"

#include "device.hpp"
#include "oled.hpp"
#include "ui_selection_list.hpp"


////////////////////////////////////////////////////////////////////////////////
class AccountsMenuImpl
{
public:
    AccountsMenuImpl(Oled& oled, DeviceInputs& userInputs, ModelStorage<Account>&);

    void prev();
    void next();
    void select();

    void draw();

    void notifyModelStoreUpdated();

public:
    BlindCall nextMenuCb_; /// jump to form showing concrete Account
    BlindCall prevMenuCb_; /// jump back to form showed before "Accounts'

    Oled& oled_;
    DeviceInputs& userInputs_;

    ModelStorage<Account>& modelStore_;

    etl::unique_ptr<UISelectionList<ModelIterator<Account>, Account>> accSl_;
};

////////////////////////////////////////////////////////////////////////////////
AccountsMenu::AccountsMenu(Oled& oled,
                           DeviceInputs& userInputs,
                           ModelStorage<Account>& modelStore)
{
    static_assert(sizeof(impl_) >= sizeof(AccountsMenuImpl), "fix AccountsMenu::impl_ size");
    new (impl_) AccountsMenuImpl(oled, userInputs, modelStore);
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
    impl()->userInputs_.set(DeviceInputs::UserAction::left, impl()->prevMenuCb_);
    impl()->userInputs_.set(DeviceInputs::UserAction::right, BlindCall::make(impl(), &AccountsMenuImpl::select));

    impl()->userInputs_.set(DeviceInputs::UserAction::up, BlindCall::make(impl(), &AccountsMenuImpl::prev));
    impl()->userInputs_.set(DeviceInputs::UserAction::down, BlindCall::make(impl(), &AccountsMenuImpl::next));
}

void
AccountsMenu::deactivate()
{
    impl()->userInputs_.unset(DeviceInputs::UserAction::left);
    impl()->userInputs_.unset(DeviceInputs::UserAction::right);

    impl()->userInputs_.unset(DeviceInputs::UserAction::up);
    impl()->userInputs_.unset(DeviceInputs::UserAction::down);
}

void
AccountsMenu::draw()
{
    // if (Serial) { Serial.println(F("AccountsMenu::draw()")); }
    impl()->draw();
}

Account
AccountsMenu::selected() const
{
    return impl()->accSl_->selected();
}

void
AccountsMenu::notifyModelStoreUpdated()
{
    impl()->notifyModelStoreUpdated();
}

////////////////////////////////////////////////////////////////////////////////
AccountsMenuImpl::AccountsMenuImpl(Oled& oled,
                                   DeviceInputs& userInputs,
                                   ModelStorage<Account>& modelStore)
    : oled_(oled)
    , userInputs_(userInputs)
    , modelStore_(modelStore)
    , accSl_(new UISelectionList<ModelIterator<Account>, Account>(oled_, "Accounts", modelStore_.cbegin(), modelStore_.cend()))
{
}

void
AccountsMenuImpl::select()
{
    nextMenuCb_();
}

void
AccountsMenuImpl::next()
{
    if (!accSl_) {
        return;
    }
    accSl_->next();
    accSl_->draw();
}

void
AccountsMenuImpl::prev()
{
    if (!accSl_) {
        return;
    }
    accSl_->prev();
    accSl_->draw();
}

void
AccountsMenuImpl::draw()
{
    if (!accSl_) {
        return;
    }
    accSl_->draw();
}

void
AccountsMenuImpl::notifyModelStoreUpdated()
{
    if (!accSl_) {
        return;
    }
    accSl_->update(modelStore_.cbegin(), modelStore_.cend());
}
