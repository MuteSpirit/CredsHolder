// Copyright (C)  2026  Ivan Efimov aka MuteSpirit <mutespirit@yandex.ru>.
//
// Permission is granted to copy, distribute and/or modify this document
// under the terms of the GNU Free Documentation License, Version 1.3
// or any later version published by the Free Software Foundation;
// with no Invariant Sections, no Front-Cover Texts, and no Back-Cover Texts.
// A copy of the license is included in the section entitled "GNU
#pragma once

#include "menu.hpp"

class AuthFormTestHelper;
class DeviceInputs;
class Oled;
class Authenticator;

////////////////////////////////////////////////////////////////////////////////
/// @details
/// Password typing use case
/// * Rotate encoder will scroll current symbol from available symbolic group
/// * Button "square" will change current group of symbols - alpha lowercase, alpha uppercase, digits, specials
/// * Button "cross" will commit current symbol
/// * Button "triangle" will erase last symbol (=== Backspace) to decrease amount of login failures due to one mistake in the middle of long password
/// * Button "circle" will commit password and trigger authentication procedure
/// * Master password will be visible during typing
///
/// Another feature:
/// * Not try auth with the same wrong password as previously to not increase "Fail Login" counter
///   * Reason: it looks like human mistake or stuck button. Lost all passwords due to such accident will be painful
class AuthForm : public Menu
{
public:
    static constexpr uint8_t passwordSize = 32; // required aes256 key size

    enum class SymbolGroup : uint8_t
    {
        alphaLowerCase,
        alphaUpperCase,
        digit,
        specials,
        COUNT
    };

    AuthForm(Oled &oled, DeviceInputs &inputs, Authenticator& auth);

    void init(BlindCall successAuthCb, BlindCall failAuthCb);

    virtual void activate() override;
    virtual void deactivate() override;

protected:
    virtual void init(BlindCall successAuthCb) override;

protected:
    friend AuthFormTestHelper;

    void chooseNextSymbolGroup();

    void chooseNextSymbol();
    void choosePrevSymbol();

    void eraseLastSymbol();
    void commitSymbol();
    void commitPassword();

    char typingSymbol() const;
    uint8_t typingGroupLen() const;

protected:
    Oled &oled_;
    DeviceInputs &userInputs_;
    Authenticator& auth_;

    BlindCall successAuthCb_ {BlindCall::stub()};
    BlindCall failAuthCb_ {BlindCall::stub()};

    uint8_t typingSymbolIdx_ {0};
    const char *typingChars_ {nullptr};
    SymbolGroup typingSymbolGroup_ {AuthForm::SymbolGroup::alphaLowerCase};

    char password_[passwordSize];
    bool allowTryAuth_ {false}; /// auth try will happen only if password is not empty and if was not recognized as wrong
};
