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
#include "auth.hpp"
#include <string.h>

#include "creds_holder.hpp"
#include <SHA256.h>

PasswordWandAuth::PasswordWandAuth()
{
    memset(salt_, 0, hashSize);
    memset(saltedPasswordHash_, 0, hashSize);
}

void
PasswordWandAuth::init(const char *salt, const uint8_t saltSize,
                       const uint8_t *saltedPasswordHash, const uint8_t saltedPasswordHashSize)
{
    strncpy(salt_, salt, saltSize <= hashSize ? saltSize : hashSize);
    memcpy(saltedPasswordHash_, saltedPasswordHash, saltedPasswordHashSize <= hashSize ? saltedPasswordHashSize : hashSize);
}

bool
PasswordWandAuth::auth(const char *password, const uint8_t len)
{
    char saltedPassword[hashSize * 2];
    memset(saltedPassword, 0, sizeof(saltedPassword));

    const uint8_t usedPassLen = len <= hashSize ? len : hashSize;
    strncpy(saltedPassword, password, usedPassLen);
    strncpy(saltedPassword + usedPassLen, salt_, hashSize * 2 - usedPassLen);

    uint8_t hash[hashSize];
    memset(hash, 0, sizeof(hash));

    SHA256 hasher;
    hasher.reset();
    hasher.update(saltedPassword, strlen(saltedPassword));
    hasher.finalize(hash, PasswordWandAuth::hashSize);

    return !memcmp(saltedPasswordHash_, hash, hashSize);
}
