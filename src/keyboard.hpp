// Copyright (C)  2026  Ivan Efimov aka MuteSpirit <mutespirit@yandex.ru>.
//
// Permission is granted to copy, distribute and/or modify this document
// under the terms of the GNU Free Documentation License, Version 1.3
// or any later version published by the Free Software Foundation;
// with no Invariant Sections, no Front-Cover Texts, and no Back-Cover Texts.
// A copy of the license is included in the section entitled "GNU
// Free Documentation License".
#pragma once

#include <inttypes.h>

class Keyboard
{
public:
    virtual bool print(const char* s) = 0;
    virtual bool push(const uint8_t key_code) = 0;
    virtual bool push_tab() = 0;

protected:
    virtual ~Keyboard() = default;
};
