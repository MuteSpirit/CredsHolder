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
#include "account.hpp"
#include <string.h>

Account::Account()
    : id(0)
    , name("")
    , url("")
    , username("")
    , password("")
{}

Account::Account(typename Account::ObjIndex idVal,
                 const char* nameVal,
                 const char* urlVal,
                 const char* usernameVal,
                 const char* passwordVal)
    : id(idVal)
{
    if (nameVal) {
        strncpy(name, nameVal, sizeof(name));
    }
    if (urlVal) {
        strncpy(url, urlVal, sizeof(url));
    }
    if (usernameVal) {
        strncpy(username, usernameVal, sizeof(username));
    }
    if (passwordVal) {
        strncpy(password, passwordVal, sizeof(password));
    }
}


Account::operator const char*()
{
    return name;
}

void
Account::reset()
{
    id = 0;
    memset(name, 0, sizeof(name));
    memset(url, 0, sizeof(url));
    memset(username, 0, sizeof(username));
    memset(password, 0, sizeof(password));
}

bool
operator==(const Account& lhs, const Account& rhs)
{
    return !strncmp(lhs.name, rhs.name, sizeof(lhs.name))
        && !strncmp(lhs.url, rhs.url, sizeof(lhs.url))
        && !strncmp(lhs.username, rhs.username, sizeof(lhs.username))
        && !strncmp(lhs.password, rhs.password, sizeof(lhs.password));
}

////////////////////////////////////////////////////////////////////////////////
template<>
char *
get_key_ptr(Account &o)
{
    return o.name;
}

template<>
uint8_t
get_key_size<Account>()
{
    return sizeof(Account::name);
}

template<>
ptrdiff_t
get_key_offset<Account>()
{
    return offsetof(Account, name);
}
