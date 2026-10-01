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
#include "account.hpp"
#include <string.h>

// Must be included as the last header to avoid troubles with macro "test"
// when such word is used in headers above
#include <AUnitVerbose.h>

test(account_ctor)
{
    Account acc;
    acc_ctor(acc);

    assertStringCaseEqual("", acc.name);
    assertStringCaseEqual("", acc.username);
    assertStringCaseEqual("", acc.password);
};

test(account_equation_operator)
{
    Account lhs;
    acc_ctor(lhs);

    Account rhs;
    acc_ctor(rhs);

    assertTrue(lhs == rhs);

    strncpy(rhs.name, "account", sizeof(rhs.name));
    assertFalse(lhs == rhs);

    acc_ctor(rhs);
    strncpy(rhs.username, "login", sizeof(rhs.username));
    assertFalse(lhs == rhs);

    acc_ctor(rhs);
    strncpy(rhs.password, "passwd", sizeof(rhs.password));
    assertFalse(lhs == rhs);
};

test(account_operator_const_char_ptr)
{
    Account acc;
    acc_ctor(acc);
    
    constexpr const char* expectedAccName = "testAcc";
    strcpy(acc.name, expectedAccName);

    assertStringCaseEqual(expectedAccName, static_cast<const char*>(acc));
};

#endif // EPOXY_DUINO
