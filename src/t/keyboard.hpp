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

#if defined(EPOXY_DUINO)
#include <inttypes.h>
#include "../keyboard.hpp"

class ImitatedKeyboard : public Keyboard
{
public:
    virtual bool print(const char* s) override;
    virtual bool push(const uint8_t keyCode) override;
    virtual bool push_tab() override;

    const char* istream() const;

protected:
    etl::string<255> istream_{""};
};

#endif // defined(EPOXY_DUINO)
