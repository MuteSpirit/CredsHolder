// Copyright (C)  2026  Ivan Efimov aka MuteSpirit <mutespirit@yandex.ru>.
//
// Permission is granted to copy, distribute and/or modify this document
// under the terms of the GNU Free Documentation License, Version 1.3
// or any later version published by the Free Software Foundation;
// with no Invariant Sections, no Front-Cover Texts, and no Back-Cover Texts.
// A copy of the license is included in the section entitled "GNU
// Free Documentation License".
#include "auth_form.hpp"
#include "device_inputs.hpp"
#include "oled.hpp"
#include "auth.hpp"


static const char alphaLowerChars[] = "abcdefghijklmnopqrstuvwxyz"; 
static const char alphaUpperChars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"; /// must be the same lenth as alphaLowerChars and sequence of symbols must be the same
static const char digitChars[] = "0123456789"; 
static const char specialChars[] = " /?><,:';|}{][+_)(*&^%$#!~=-@. ";

AuthForm::AuthForm(Oled &oled, DeviceInputs &userInputs, Authenticator& auth)
    : oled_(oled)
    , userInputs_(userInputs)
    , auth_(auth)
    , typingChars_(alphaLowerChars)
{
    memset(password_, 0, sizeof(password_));
}

void
AuthForm::init(BlindCall successAuthCb, BlindCall failAuthCb)
{
    init(successAuthCb);
    successAuthCb_ = successAuthCb;
    failAuthCb_ = failAuthCb;
}

void
AuthForm::init(BlindCall successAuthCb)
{
    successAuthCb_ = successAuthCb;
}

void
AuthForm::activate()
{
    memset(password_, 0, sizeof(password_));
    allowTryAuth_ = false;

    userInputs_.set(DeviceInputs::UserAction::right, BlindCall::make(this, &AuthForm::commitPassword));
    userInputs_.set(DeviceInputs::UserAction::down, BlindCall::make(this, &AuthForm::commitSymbol));
    userInputs_.set(DeviceInputs::UserAction::left, BlindCall::make(this, &AuthForm::chooseNextSymbolGroup));
    userInputs_.set(DeviceInputs::UserAction::up, BlindCall::make(this, &AuthForm::eraseLastSymbol));

    draw();
}

void
AuthForm::deactivate()
{
    memset(password_, 0, sizeof(password_));
    allowTryAuth_ = false;

    userInputs_.unset(DeviceInputs::UserAction::right);
    userInputs_.unset(DeviceInputs::UserAction::down);
    userInputs_.unset(DeviceInputs::UserAction::left);

    userInputs_.set(DeviceInputs::UserAction::up, successAuthCb_);

    oled_.clear();
}

// AuthForm::SymbolGroup& 
// operator++(AuthForm::SymbolGroup& g)
// {
//     uint8_t next = (static_cast<uint8_t>(g) + 1) % static_cast<uint8_t>(AuthForm::SymbolGroup::COUNT);
//     g = static_cast<AuthForm::SymbolGroup>(next);
//     return g;
// }

void
AuthForm::chooseNextSymbolGroup()
{
    switch (typingSymbolGroup_) {
        case SymbolGroup::alphaLowerCase:
            typingSymbolGroup_ = SymbolGroup::alphaUpperCase;
            typingChars_ = alphaUpperChars;
            break;
        case SymbolGroup::alphaUpperCase:
            typingSymbolIdx_ = 0;
            typingSymbolGroup_ = SymbolGroup::digit;
            typingChars_ = digitChars;
            break;
        case SymbolGroup::digit:
            typingSymbolIdx_ = 0;
            typingSymbolGroup_ = SymbolGroup::specials;
            typingChars_ = specialChars;
            break;
        case SymbolGroup::specials:
            typingSymbolIdx_ = 0;
            typingSymbolGroup_ = SymbolGroup::alphaLowerCase;
            typingChars_ = alphaLowerChars;
            break;
        default:
            break;
    };
}

void
AuthForm::chooseNextSymbol()
{
    auto next = (typingSymbolIdx_ + 1) % typingGroupLen();
    typingSymbolIdx_ = next;
}

void
AuthForm::choosePrevSymbol()
{
    typingSymbolIdx_ = typingSymbolIdx_ > 0 ? typingSymbolIdx_ - 1 : typingGroupLen() - 1;
}

uint8_t
AuthForm::typingGroupLen() const
{
    // " - 1" is needed because strlen don't take ending zero but
    // statement like "sizeof(array) / sizeof(array[0])" count last '\0' too
    switch (typingSymbolGroup_) {
        case SymbolGroup::alphaLowerCase:
            return sizeof(alphaLowerChars) / sizeof(alphaLowerChars[0]) - 1;
        case SymbolGroup::alphaUpperCase:
            return sizeof(alphaUpperChars) / sizeof(alphaUpperChars[0]) - 1;
        case SymbolGroup::digit:
            return sizeof(digitChars) / sizeof(digitChars[0]) - 1;
        case SymbolGroup::specials:
            return sizeof(specialChars) / sizeof(specialChars[0]) - 1;
        default:
            return 0;
            break;
    };
}

void
AuthForm::eraseLastSymbol()
{
    const size_t len = strnlen(password_, passwordSize - 1);
    password_[len - 1] = '\0';

    if (0 == len) {
        allowTryAuth_ = false;
    }
}

void
AuthForm::commitSymbol()
{
    const size_t len = strnlen(password_, passwordSize - 1);
    password_[len] = typingSymbol();
    password_[len + 1] = '\0';
    allowTryAuth_ = true;
}

void
AuthForm::commitPassword()
{
    if (!allowTryAuth_) {
        return;
    }

    if (auth_.auth(password_, strnlen(password_, passwordSize - 1))) {
        successAuthCb_();
    } else {
        failAuthCb_();
    }
    // repeating even success authentication looks wrong
    allowTryAuth_ = false;
}

char
AuthForm::typingSymbol() const
{
    return typingChars_[typingSymbolIdx_];
}

void
AuthForm::draw()
{
    oled_.clear();
    oled_.home();
    oled_.println(typingChars_);
}
