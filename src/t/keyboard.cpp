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
#if defined(EPOXY_DUINO)
#include "keyboard.hpp"

bool
ImitatedKeyboard::print(const char* s)
{
    (void)s;
    return true;
}

bool
ImitatedKeyboard::push(const uint8_t keyCode)
{
    (void)keyCode;
    return true;
}

bool
ImitatedKeyboard::push_tab()
{
    return true;
}

#endif // defined(EPOXY_DUINO)
