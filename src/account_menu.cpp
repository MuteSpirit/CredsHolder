// Copyright (C)  2026  Ivan Efimov aka MuteSpirit <mutespirit@yandex.ru>.
//
// Permission is granted to copy, distribute and/or modify this document
// under the terms of the GNU Free Documentation License, Version 1.3
// or any later version published by the Free Software Foundation;
// with no Invariant Sections, no Front-Cover Texts, and no Back-Cover Texts.
// A copy of the license is included in the section entitled "GNU
// Free Documentation License".
#include "account_menu.hpp"
// #include <new>

#include "etl/memory.h"
#include "etl/string.h"
#include "etl/iterator.h"

#include "oled.hpp"
#include "device_inputs.hpp"
#include "model.hpp"
#include "keyboard.hpp"
#include "ui_selection_list.hpp"

////////////////////////////////////////////////////////////////////////////////
class AccountField
{
public:
    /// !!!! update "first()" and "last" on change Field enum
    enum class Field : int8_t
    {
        // url,
        login = 0,
        password,
        END
    };

public:
    AccountField(const Account* acc, Field field);
    AccountField(const AccountField&) = default;

    AccountField& operator=(const AccountField& rhs);

    AccountField operator++(); // prefix increment
    AccountField operator++(int); // postfix increment

    AccountField operator--(); // prefix decrement
    AccountField operator--(int); // postfix decrement

    bool operator==(const AccountField& rhs) const;
    bool operator!=(const AccountField& rhs) const;

    /// for printing, viewing to user
    explicit operator const char*() const;

    /// return real value, stored in field to type it as keyboard
    const char* operator*() const;

    static Field first() { return Field::login; }
    static Field last() { return Field::password; }
    static Field end() { return Field::END; }

protected:
    void updatePrintStr();

protected:
    const Account* acc_ {nullptr};
    Field field_ {Field::END};
    etl::string<64> s_{""};
};

////////////////////////////////////////////////////////////////////////////////
class AccountFieldIterator : public etl::iterator<etl::bidirectional_iterator_tag, AccountField>
{
public:
    AccountFieldIterator(const AccountFieldIterator& rhs);
    AccountFieldIterator(const AccountFieldIterator&& rhs);

    AccountFieldIterator& operator=(const AccountFieldIterator&);

    const AccountField& operator*() const;

    AccountFieldIterator operator++(); // prefix increment
    AccountFieldIterator operator++(int); // postfix increment

    AccountFieldIterator operator--(); // prefix decrement
    AccountFieldIterator operator--(int); // postfix decrement

    bool operator==(const AccountFieldIterator& rhs) const;
    bool operator!=(const AccountFieldIterator& rhs) const;

protected:
    /// Create iterator via factory methods "begin" and "end"
    explicit AccountFieldIterator(const Account*, AccountField::Field);

    friend AccountFieldIterator accItBegin(const Account*);
    friend AccountFieldIterator accItEnd(const Account*);

protected:
    AccountField field_;
};

AccountFieldIterator accItBegin(const Account*);
AccountFieldIterator accItEnd(const Account*);

////////////////////////////////////////////////////////////////////////////////
class AccountMenuImpl
{
public:
    AccountMenuImpl(Oled& oled, DeviceInputs& userInputs, Keyboard& keyboard);

    void prev();
    void next();
    void select();

public:
    BlindCall nextMenuCb_; /// jump to form showing concrete Account
    BlindCall prevMenuCb_; /// jump back to form showed before "Accounts'

    Oled& oled_;
    DeviceInputs& userInputs_;
    Keyboard& keyboard_;

    /// Own Account instance to avoid hanging Account pointers in AccountFieldIterator objects
    Account acc_;

    etl::unique_ptr<UISelectionList<AccountFieldIterator, AccountField>> fieldsSl_ {nullptr};
};


////////////////////////////////////////////////////////////////////////////////
AccountMenu::AccountMenu(Oled& oled, DeviceInputs& userInputs, Keyboard& keyboard)
{
    new (impl_) AccountMenuImpl(oled, userInputs, keyboard);
}

AccountMenuImpl::AccountMenuImpl(Oled& oled, DeviceInputs& userInputs, Keyboard& keyboard)
    : oled_(oled)
    , userInputs_(userInputs)
    , keyboard_(keyboard)
{}

AccountMenuImpl*
AccountMenu::impl()
{
    return reinterpret_cast<AccountMenuImpl*>(impl_);
}

const AccountMenuImpl*
AccountMenu::impl() const
{
    return reinterpret_cast<const AccountMenuImpl*>(impl_);
}

////////////////////////////////////////////////////////////////////////////////
void
AccountMenu::init(BlindCall nextMenuCb, BlindCall prevMenuCb)
{
    impl()->nextMenuCb_ = nextMenuCb;
    impl()->prevMenuCb_ = prevMenuCb;
}

void
AccountMenu::activate()
{
    impl()->userInputs_.set(DeviceInputs::UserAction::up, BlindCall::make(impl(), &AccountMenuImpl::prev));
    impl()->userInputs_.set(DeviceInputs::UserAction::down, BlindCall::make(impl(), &AccountMenuImpl::next));

    impl()->userInputs_.set(DeviceInputs::UserAction::left, impl()->prevMenuCb_);
    impl()->userInputs_.set(DeviceInputs::UserAction::right, BlindCall::make(impl(), &AccountMenuImpl::select));
}

void
AccountMenu::deactivate()
{
    impl()->userInputs_.unset(DeviceInputs::UserAction::up);
    impl()->userInputs_.unset(DeviceInputs::UserAction::down);
    impl()->userInputs_.unset(DeviceInputs::UserAction::left);
    impl()->userInputs_.unset(DeviceInputs::UserAction::right);

    memset(&impl()->acc_, 0, sizeof(Account));
}

void
AccountMenu::draw()
{
    impl()->fieldsSl_->draw();
}

void
AccountMenu::account(const Account& acc)
{
    // make a copy to own this instance and avoid invalid pointers to it in iterators created for SelectionList
    impl()->acc_ = acc;

    impl()->fieldsSl_.reset(new UISelectionList<AccountFieldIterator, AccountField>(impl()->oled_, 
                static_cast<const char*>(*accItEnd(&impl()->acc_)), // title
                accItBegin(&impl()->acc_),
                accItEnd(&impl()->acc_)));
}

void
AccountMenu::account(const Account&& acc)
{
    impl()->acc_ = etl::move(acc);

    impl()->fieldsSl_.reset(new UISelectionList<AccountFieldIterator, AccountField>(impl()->oled_, "Account", accItBegin(&impl()->acc_), accItEnd(&impl()->acc_)));
}

const Account&
AccountMenu::account() const
{
    return impl()->acc_;
}

void
AccountMenuImpl::prev()
{
    fieldsSl_->prev();
}

void
AccountMenuImpl::next()
{
    fieldsSl_->next();
}

void
AccountMenuImpl::select()
{
    keyboard_.print(static_cast<const char*>(fieldsSl_->selected()));
}

////////////////////////////////////////////////////////////////////////////////
AccountField::AccountField(const Account* acc, Field field)
    : acc_(acc)
    , field_(field)
{
    updatePrintStr();
}

AccountField&
AccountField::operator=(const AccountField& rhs)
{
    acc_ = rhs.acc_;
    field_ = rhs.field_;
    s_ = rhs.s_;

    return *this;
}

void AccountField::updatePrintStr()
{
    if (!acc_) {
        s_ = "";
        return;
    }

    switch (field_) {
        case Field::login:
            s_ = "L: ";
            s_ += acc_->username;
            break;

        case Field::password:
            s_ = "P: ";
            s_ += acc_->password;
            break;

        default:
            s_ = "N: ";
            s_ += acc_->name;
            break;
    }
}

AccountField
AccountField::operator++()
{
    if (end() != field_) {
        field_ = static_cast<Field>(static_cast<int8_t>(field_) + 1);
        updatePrintStr();
    }
    return *this;
}

AccountField
AccountField::operator++(int) // postfix ++
{
    AccountField tmp(*this);
    if (end() != field_) {
        field_ = static_cast<Field>(static_cast<int8_t>(field_) + 1);
        updatePrintStr();
    }
    return tmp;
}

AccountField
AccountField::operator--()
{
    if (first() != field_) {
        field_ = static_cast<Field>(static_cast<int8_t>(field_) - 1);
        updatePrintStr();
    }
    return *this;
}

AccountField
AccountField::operator--(int) // postfix --
{
    AccountField tmp(*this);
    if (first() != field_) {
        field_ = static_cast<Field>(static_cast<int8_t>(field_) - 1);
        updatePrintStr();
    }
    return tmp;
}

bool
AccountField::operator==(const AccountField& rhs) const
{
    return acc_ == rhs.acc_ && field_ == rhs.field_;
}

bool
AccountField::operator!=(const AccountField& rhs) const
{
    return acc_ != rhs.acc_ || field_ != rhs.field_;
}

const char*
AccountField::operator*() const
{
    if (!acc_) {
        return "";
    }
    switch (field_) {
        case Field::login:
            return acc_->username;

        case Field::password:
            return acc_->password;

        default:
            return acc_->name;
    }
}

AccountField::operator const char*() const
{
    return s_.c_str();
}

////////////////////////////////////////////////////////////////////////////////
AccountFieldIterator::AccountFieldIterator(const Account* acc, AccountField::Field field)
    : field_(acc, field)
{}

AccountFieldIterator::AccountFieldIterator(const AccountFieldIterator& rhs)
    : field_(rhs.field_)
{}

AccountFieldIterator::AccountFieldIterator(const AccountFieldIterator&& rhs)
    : field_(etl::move(rhs.field_))
{}

AccountFieldIterator&
AccountFieldIterator::operator=(const AccountFieldIterator& rhs)
{
    field_ = rhs.field_;
    return *this;
}

const AccountField&
AccountFieldIterator::operator*() const
{
    return field_;
}

AccountFieldIterator AccountFieldIterator::operator++()
{
    ++field_;
    return *this;
}

AccountFieldIterator AccountFieldIterator::operator++(int)
{
    AccountFieldIterator tmp(*this);
    ++field_;
    return tmp;
}

AccountFieldIterator AccountFieldIterator::operator--()
{
    --field_;
    return *this;
}

AccountFieldIterator AccountFieldIterator::operator--(int)
{
    AccountFieldIterator tmp(*this);
    --field_;
    return tmp;
}

bool AccountFieldIterator::operator==(const AccountFieldIterator& rhs) const
{
    return field_ == rhs.field_;
}

bool AccountFieldIterator::operator!=(const AccountFieldIterator& rhs) const
{
    return field_ != rhs.field_;
}

AccountFieldIterator accItBegin(const Account* acc)
{
    return AccountFieldIterator(acc, AccountField::first());
}

AccountFieldIterator accItEnd(const Account* acc)
{
    return AccountFieldIterator(acc, AccountField::end());
}
