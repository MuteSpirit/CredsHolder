// Copyright (C)  2026  Ivan Efimov aka MuteSpirit <mutespirit@yandex.ru>.
//
// Permission is granted to copy, distribute and/or modify this document
// under the terms of the GNU Free Documentation License, Version 1.3
// or any later version published by the Free Software Foundation;
// with no Invariant Sections, no Front-Cover Texts, and no Back-Cover Texts.
// A copy of the license is included in the section entitled "GNU
// Free Documentation License".
#pragma once

#include "blind_call.hpp"

class Menu
{
public:
    virtual ~Menu() {};

    virtual void init(BlindCall nextMenuCb, BlindCall prevMenuCb) = 0;

    virtual void activate() = 0;
    virtual void deactivate() = 0;

    virtual void draw() = 0;

protected:
    Menu() = default;
};
