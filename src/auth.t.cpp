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

#include "auth.hpp"
#include "AUnitVerbose.h"
#include "SHA256.h"

using namespace aunit;

class AuthenticatorTester : public TestOnce
{
protected:
    void setup() override
    {
        TestOnce::setup();

        strcpy(salt, "WithSalt");

        strcpy(saltedPassword, "PasswordWithSalt");

        memset(hash, 0, PasswordWandAuth::hashSize);

        SHA256 hasher;
        hasher.reset();
        hasher.update(saltedPassword, strlen(saltedPassword));
        hasher.finalize(hash, PasswordWandAuth::hashSize);
    }

    void teardown() override
    {
        TestOnce::teardown();
    }

    char salt[PasswordWandAuth::hashSize];
    char saltedPassword[PasswordWandAuth::hashSize];
    uint8_t hash[PasswordWandAuth::hashSize];

    PasswordWandAuth wandAuth;
};

testF(AuthenticatorTester, auth_success)
{
    wandAuth.init(salt, strlen(salt), hash, PasswordWandAuth::hashSize);
    assertTrue(wandAuth.auth("Password", strlen("Password")));
};

testF(AuthenticatorTester, auth_fail_with_another_salt)
{
    wandAuth.init("Wrong", strlen("Wrong"), hash, PasswordWandAuth::hashSize);
    assertFalse(wandAuth.auth("Password", strlen("Password")));
};

testF(AuthenticatorTester, auth_fail_with_wrong_password)
{
    wandAuth.init("WithSalt", strlen("WithSalt"), hash, PasswordWandAuth::hashSize);
    assertFalse(wandAuth.auth("WrongPassword", strlen("WrongPassword")));
};

#endif // EPOXY_DUINO
