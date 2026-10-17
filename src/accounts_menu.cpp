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

#include <Embedded_Template_Library.h>
#include <etl/memory.h>

#include "model/sql/account_storage.hpp"
#include "model/blob/account_storage.hpp"
#include "model/blob/iterator.hpp"

#include "device_inputs.hpp"
#include "oled.hpp"
#include "ui_selection_list.hpp"

namespace creds_holder {

////////////////////////////////////////////////////////////////////////////////
template<typename Storage>
class AccountsMenuImpl
{
public:
    using AccountSelectionList = UISelectionList<typename Storage::ConstIterator, Account>;

public:
    AccountsMenuImpl(Oled& oled, DeviceInputs& userInputs, Storage&);

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

    Storage& modelStore_;

    etl::unique_ptr<AccountSelectionList> accSl_;
};

////////////////////////////////////////////////////////////////////////////////
template<typename Storage>
AccountsMenu<Storage>::AccountsMenu(Oled& oled,
        DeviceInputs& userInputs,
        Storage& modelStore)
{
    static_assert(sizeof(impl_) >= sizeof(Impl), "fix AccountsMenu<Storage>::impl_ size");
    new (impl_) Impl(oled, userInputs, modelStore);
}

template<typename Storage>
typename AccountsMenu<Storage>::Impl*
AccountsMenu<Storage>::impl()
{
    return reinterpret_cast<AccountsMenuImpl<Storage>*>(impl_);
}

template<typename Storage>
const typename AccountsMenu<Storage>::Impl*
AccountsMenu<Storage>::impl() const
{
    return reinterpret_cast<const AccountsMenuImpl<Storage>*>(impl_);
}

template<typename Storage>
void
AccountsMenu<Storage>::init(BlindCall nextMenuCb, BlindCall prevMenuCb)
{
    impl()->nextMenuCb_ = nextMenuCb;
    impl()->prevMenuCb_ = prevMenuCb;
}

template<typename Storage>
void
AccountsMenu<Storage>::activate()
{
    impl()->userInputs_.set(DeviceInputs::UserAction::left, impl()->prevMenuCb_);
    impl()->userInputs_.set(DeviceInputs::UserAction::right, BlindCall::make(impl(), &Impl::select));

    impl()->userInputs_.set(DeviceInputs::UserAction::up, BlindCall::make(impl(), &Impl::prev));
    impl()->userInputs_.set(DeviceInputs::UserAction::down, BlindCall::make(impl(), &Impl::next));

    impl()->oled_.setFont(u8x8_font_8x13_1x2_f);
}

template<typename Storage>
void
AccountsMenu<Storage>::deactivate()
{
    impl()->userInputs_.unset(DeviceInputs::UserAction::left);
    impl()->userInputs_.unset(DeviceInputs::UserAction::right);

    impl()->userInputs_.unset(DeviceInputs::UserAction::up);
    impl()->userInputs_.unset(DeviceInputs::UserAction::down);
}

template<typename Storage>
void
AccountsMenu<Storage>::draw()
{
    // if (Serial) { Serial.println(F("AccountsMenu<Storage>::draw()")); }
    impl()->draw();
}

template<typename Storage>
Account
AccountsMenu<Storage>::selected() const
{
    return impl()->accSl_->selected();
}

template<typename Storage>
void
AccountsMenu<Storage>::notifyModelStoreUpdated()
{
    impl()->notifyModelStoreUpdated();
}

////////////////////////////////////////////////////////////////////////////////
template<typename Storage>
AccountsMenuImpl<Storage>::AccountsMenuImpl(Oled& oled,
                                   DeviceInputs& userInputs,
                                   Storage& modelStore)
    : oled_(oled)
    , userInputs_(userInputs)
    , modelStore_(modelStore)
    , accSl_(new AccountSelectionList(oled_, "Accounts", modelStore_.cbegin(), modelStore_.cend()))
{
}

template<typename Storage>
void
AccountsMenuImpl<Storage>::select()
{
    nextMenuCb_();
}

template<typename Storage>
void
AccountsMenuImpl<Storage>::next()
{
    if (!accSl_) {
        return;
    }
    accSl_->next();
    accSl_->draw();
}

template<typename Storage>
void
AccountsMenuImpl<Storage>::prev()
{
    if (!accSl_) {
        return;
    }
    accSl_->prev();
    accSl_->draw();
}

template<typename Storage>
void
AccountsMenuImpl<Storage>::draw()
{
    if (!accSl_) {
        return;
    }
    accSl_->draw();
}

template<typename Storage>
void
AccountsMenuImpl<Storage>::notifyModelStoreUpdated()
{
    if (!accSl_) {
        return;
    }
    accSl_->update(modelStore_.cbegin(), modelStore_.cend());
}

////////////////////////////////////////////////////////////////////////////////
// Let's instantiate for concrete storages to keep control on class usage
template class AccountsMenu<AccountSqlStorage>;
template class AccountsMenu<AccountBlobStorage>;

} // namespace creds_holder
